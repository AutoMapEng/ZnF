#pragma once
#ifndef WIN32
#include <zfcore/filename.h>
#include "zfsapi.h"

// Build Boost version for Visual Studio 2012 (and below) or if Boost version is
// enforced (macro FILE_LOCK_FORCE_USE_BOOST is defined)
#if (defined(_MSC_VER) && _MSC_VER <= 1700) \
    || defined(FILE_LOCK_FORCE_USE_BOOST)
#define FILE_LOCK_USE_BOOST
#endif

#ifdef FILE_LOCK_USE_BOOST
#include <boost/atomic/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#else
#include <atomic>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <thread>
#endif


namespace zfs {

namespace locks_detail {

/// duration to sleep between locking attempts [ms]
static const auto try_retry_time_ms = 5u;

#ifdef FILE_LOCK_USE_BOOST

    template<class Rep, class Period>
    auto make_timepoint(const boost::chrono::duration<Rep, Period>& timeout)
        -> decltype(boost::chrono::steady_clock::now())
    {
        using boost::chrono::steady_clock;
        auto dur = boost::chrono::duration_cast<
                steady_clock::duration>(timeout);
        if (boost::ratio_greater<steady_clock::period, Period>())
            ++dur;

        return steady_clock::now() + dur;
    }

    template<class LockDev, class Clock, class Dur>
    bool try_lock_obj_until(
            LockDev& lock_dev, bool shared,
            const boost::chrono::time_point<Clock, Dur>& until)
    {
        using boost::chrono::steady_clock;
        const auto _until = boost::chrono::time_point_cast<
                steady_clock::duration>(until);

        // lambda: call either try_lock() or try_lock_shared depending on
        // parameter shared
        const auto try_lock = [&lock_dev, shared] {
            return shared
                    ? lock_dev.try_lock_shared()
                    : lock_dev.try_lock();
        };

        // call try_lock() repeatadly. No better solution for now, except
        // using complicated solutions like inotify...
        do {
            if (try_lock())
                return true;

            // don't let the CPU to run hot...
            boost::this_thread::sleep_for(
                        boost::chrono::milliseconds(try_retry_time_ms));
        } while (_until > steady_clock::now());

        return try_lock();
    }

#else // FILE_LOCK_USE_BOOST

    template<class Rep, class Period>
    auto make_timepoint(const std::chrono::duration<Rep, Period>& timeout)
    {
        using std::chrono::steady_clock;

        auto dur = std::chrono::duration_cast<steady_clock::duration>(timeout);
        if (std::ratio_greater<steady_clock::period, Period>())
            ++dur;

        return steady_clock::now() + dur;
    }

    template<class LockDev, class Clock, class Dur>
    bool try_lock_obj_until(
            LockDev& lock_dev, bool shared,
            const std::chrono::time_point<Clock, Dur>& until)
    {
        using std::chrono::steady_clock;
        const auto _until = std::chrono::time_point_cast<
                steady_clock::duration>(until);

        // lambda: call either try_lock() or try_lock_shared depending on
        // parameter shared
        const auto try_lock = [&lock_dev, shared] {
            return shared
                    ? lock_dev.try_lock_shared()
                    : lock_dev.try_lock();
        };

        // call try_lock() repeatadly. No better solution for now, except
        // using complicated solutions like inotify...
        do {
            if (try_lock())
                return true;

            // don't let the CPU to run hot...
            std::this_thread::sleep_for(
                        std::chrono::milliseconds(try_retry_time_ms));
        } while (_until > steady_clock::now());

        return try_lock();
    }
#endif // FILE_LOCK_USE_BOOST

} // namespace locks_detail

/**
 * @brief Creates a separate .lock file to inform other processes/threads about
 *        the lock.
 *
 * Can well be used in conjunction with std::lock_guard<> or std::unique_lock<>
 * or std::shared_lock<> (c++14).
 *
 * shared locks may get upgraded to exclusive locks and downgraded again.
 *
 * @warning The NamedFileLock isn't intended for inter thread locking and isn't
 *    "thread safe" by itself. If you like to synchronize on the same lock
 *    file from different threads, you need as many independent NamedFileLock
 *    objects (refering the same lock file) as threads. Alternatively you may
 *    like to wrap NamedFileLock together with some multithreading aware mutex.
 *
 * The lock files content has following format:
 *     {Host-Name}:{PID of creating process}:{path to executable}
 *
 * This way there should be enough information to automatically remove stale
 * locks (e.g. crashed program or invalid lock handling). However, this is
 * not absolutely fool proof. Unintended cleanups may happen when e.g.:
 *
 * -> different host with same name (bad) has by chance same PID/exe combination
 * -> Subprocess has taken lock (and this is intended). PID/exe combination
 *    will be gone and thus considered stale.
 * -> Possibly many other cases...
 */
class ZFS_API NamedFileLock
{
#ifdef FILE_LOCK_USE_BOOST
    typedef boost::mutex mutex_t;
    typedef boost::lock_guard<mutex_t> guard;
#else
    using mutex_t = std::mutex;
    using guard = std::lock_guard<mutex_t>;
#endif


    // don't allow copies!
    NamedFileLock(const NamedFileLock&);
    NamedFileLock& operator=(const NamedFileLock&);


    // however: Moves could be possibly implemented. But disabled because
    //          ~NamedFileLock() is there!
public:

    /**
     * @brief Create the LockFileMutex. The lock file itself is not yet created
     */
    NamedFileLock(const zf::FileName& file);

    /**
     * @brief Destroys the LockFileMutex.
     *
     * The lock is NOT released if the LockFileMutex was previously aquired!
     */
    ~NamedFileLock();

    /**
     * @brief Aquire shared ownership of the lock
     *
     * If another object is holding the lock in exclusive ownership mode, a
     * call to lock_shared will block exceution until shared ownership can
     * be aquired (exclusive ownership is released).
     *
     * If lock_shared is called on an object that already holds exclusive
     * or shared ownership an exception of type std::system_error is thrown.
     *
     * Upon success a file named *.lock.{unique_id} is created
     *
     * @throws std::system_error if the underlying system call fails or the
     *         lock is already held (either shared or exclusive)
     */
    void lock_shared();

    /**
     * @brief Release the lock from shared ownership. The lock must be held in
     *        shared mode.
     *
     * @throws std::system_error if the underlying system call fails or the lock
     *         is not held in shared mode.
     */
    void unlock_shared();

    /**
     * @brief Tries to lock the object in shared mode. Returns immediately
     *
     * On success the lock true is returned and the lock is aquired in shared
     * mode.
     *
     * If the lock is already ownded exclusively or shared an exception of type
     * std::system_error is thrown.
     *
     * @return true if the lock was successfully aquired in shared mode
     * @throws std::system_error if underlying system calls fails or the lock
     *         is already aquired.
     */
    bool try_lock_shared();

    /**
     * @brief Tries to lock the object in shared mode. Blocks for specified
     *        duration.
     *
     * @see try_lock_shared_until()
     *
     * @return true if lock was aquired successfully
     * @throws std::sytem_error on error or misuse
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Rep, class Period>
    bool try_lock_shared_for(
            const boost::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_shared_until(locks_detail::make_timepoint(timeout));
    }
#else
    template<class Rep, class Period>
    bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_shared_until(locks_detail::make_timepoint(timeout));
    }
#endif

    /**
     * @brief Tries to lock the object in shared mode. Blocks until specified
     *        point in time.
     *
     * Calls try_lock_shared() repeatadly. Fails if the timeout occurs. The
     * same restrictions as for try_lock_shared() apply.
     *
     * @see try_lock_shared()
     *
     * @return true if lock was aquired successfully
     * @throws std::system_error on error or misuse
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Clock, class Dur>
    bool try_lock_shared_until(
            const boost::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, true, until);
    }
#else
    template<class Clock, class Dur>
    bool try_lock_shared_until(const std::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, true, until);
    }
#endif

    /**
     * @brief Aquire the lock
     *
     * If another thread/process already owns the lock file, the call to this
     * function will block until the lock is aquired.
     *
     * If lock() is called twice on the same NamedFileLock object
     * std::system_error with error condition resource_deadlock_would_occur
     * is thrown.
     *
     * The lock may alreay held a lock in shared mode. In this case the lock
     * is upgraded to exclusive ownership.
     *
     * Upon success a *.lock file is created
     *
     * @throws std::system_error in case of error conditions, including those
     *   creating the lock file itself.
     */
    void lock();

    /**
     * @brief Unlocks the LockFileMutex
     *
     * The *.lock file is deleted and thus the lock released.
     * Unlocking an alreay unlocked NamedFileMutex object results in an
     * std::system_error with error condition operation_not_permitted to
     * be thrown.
     *
     * Unlocking a NamedFileMutex object which was locked while holding a
     * shared mutex is downgraded to shared ownership.
     *
     * @throws std::system_error in case of error conditions, including those
     *   closing or deleting the file
     */
    void unlock();

    /**
     * @brief Tries to aquire the lock
     *
     * A call to this function doesn't block. If the lock could be aquired the
     * lock is taken and true is returned. If the file lock was already locked
     * by another process or thread false is returned instead and the lock isn't
     * aquried.
     *
     * If try_lock() is called on an already locked NamedFileLock object,
     * std::system_error with error condition resource_deadlock_would_occur
     * is thrown
     *
     * @return true uppon successfull locking
     * @throws std::system_error in case of error conditions
     */
    bool try_lock();

    /**
     * @brief Try to lock for a specified duration
     *
     * @throws if try_lock() throws
     * @return Returns true if the lock has been aquired
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Rep, class Period>
    bool try_lock_for(const boost::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_until(locks_detail::make_timepoint(timeout));
    }
#else
    template<class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_until(locks_detail::make_timepoint(timeout));
    }
#endif // FILE_LOCK_USE_BOOST

    /**
     * @brief Try to lock until a specified time point
     *
     * @throws if try_lock() throws
     * @return Returns true if the lock has been aquired
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Clock, class Dur>
    bool try_lock_until(const boost::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, false, until);
    }
#else
    template<class Clock, class Dur>
    bool try_lock_until(const std::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, false, until);
    }
#endif // FILE_LOCK_USE_BOOST

private:
    zf::FileName m_share; ///< path to the shared lock file
    zf::FileName m_file;  ///< path to the lock file
    int m_fd_ex;              ///< file descriptor for exclusive locking
    int m_fd_sh;              ///< file descriptor for shared locking

    unsigned m_sh_locks;      ///< number of shared locks (this object)
    mutex_t  m_sh_mtx;        ///< mutex to establish shared locks
};

#ifndef _WIN32

/**
 * @brief File locking facility which doesn't create an extra companion file
 *
 * Not supported on Windows hence Windows only enforced supports byte range
 * locks on files.
 */
class ZFS_API UnnamedFileLock
{
#ifdef FILE_LOCK_USE_BOOST
    using mutex_t = boost::mutex;
    using guard = boost::lock_guard<mutex_t>;
#else
    using mutex_t = std::mutex;
    using guard = std::lock_guard<mutex_t>;
#endif

public:

    /**
     * @brief Creates a UnnamedFileLock for the file @c file
     *
     * @note does not actually lock the file.
     * @note the file must be present and obenable for read.
     * @throws std::system_error if the file can't get opened
     * @param file File this lock is associated with
     */
    UnnamedFileLock(const zf::FileName& file);

    // don't allow copies! Managing m_fd
    // however: moves would be possible
    UnnamedFileLock(const UnnamedFileLock&) = delete;
    UnnamedFileLock& operator=(const UnnamedFileLock&) = delete;

    // however: Moves could be possibly implemented. But disabled because
    //          ~UnnamedFileLock() is there!

    /**
     * @brief Destroys the UnnamedFileLock
     *
     * The file descriptor gets closed and thus the lock released
     */
    ~UnnamedFileLock();

    /**
     * @brief Acquires shared ownership of the lock
     *
     * If another thread is holding the lock in exclusive ownership, a call to
     * lock_shared will block execution until shared ownership can be aquired.
     *
     * If lock_shared is called on an object that already holds exclusive
     * ownership an exception of type std::system_error is thrown
     *
     * @throws std::system_error if the underlying system call fails or the
     *         lock is already held in exclusive mode
     */
    void lock_shared();

    /**
     * @brief Release the lock from shared ownership. The lock must be held in
     * shared mode.
     *
     * @throws std::system_error if the underlying system call fails or the
     *         lock is not in shared mode
     */
    void unlock_shared();

    /**
     * @brief Tries to lock the object in shared mode. Returns immediately.
     *
     * On success the lock is aquired and true is retuned, otherwise false.
     *
     * If the lock is already owned exclusively an exception of type
     * std::system_error is thrown
     *
     * @return true if lock is aquired in shared mode
     * @throws std::system_error if the underlying system call fails or the
     *         lock is already held in exclusive mode
     */
    bool try_lock_shared()
    {
        return try_lock(true);
    }

#ifdef FILE_LOCK_USE_BOOST
    template<class Rep, class Period>
    bool try_lock_shared_for(
            const boost::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_shared_until(locks_detail::make_timepoint(timeout));
    }
#else
    template<class Rep, class Period>
    bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_shared_until(locks_detail::make_timepoint(timeout));
    }
#endif

#ifdef FILE_LOCK_USE_BOOST
    template<class Clock, class Dur>
    bool try_lock_shared_until(
            const boost::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, true, until);
    }
#else
    template<class Clock, class Dur>
    bool try_lock_shared_until(const std::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, true, until);
    }
#endif

    /**
     * @brief Aquire the exclusive lock on the associated file
     *
     * Blocks if the file is already locked, either shared or exclusive
     * If already a shared lock is held, it is upgraded to exclusive mode.
     * In the upgrade case a call to unlock() will downgrade to shared mode
     * again.
     *
     * @throws std::system_error if the underlying system calls fails
     */
    void lock();

    /**
     * @brief Removes the exclusive lock on the associated file
     *
     * If the associated file was previously locked in shared mode,
     * this call downgrades to shared mode, else the lock is released
     * completely.
     *
     * @throws std::system_error if the underlying system calls fails
     */
    void unlock();

    /**
     * @brief Tries to aquire the lock but doesn't block
     *
     * If this function returns true, the lock has been successfully aquired and
     * can be seen as locked.
     *
     * @note No multithreading locking here! try_lock() or lock() on the same
     *       object will work, if the underlying file descriptor is already
     *       locked! However: The same calls on different objects
     *       (different file descriptors but same file name) will work.
     *
     * @return true if lock has been aquired
     * @throws std::system_error if the underlying system call fails
     */
    bool try_lock()
    {
        return try_lock(false);
    }

    /**
     * @brief Tries to aquire unique lock for a specified duration
     *
     * @param timeout time to try lock for. If timeout occurs, false is returned
     * @throws if try_lock() throws
     * @return Returns true if the lock has been aquired
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Rep, class Period>
    bool try_lock_for(const boost::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_until(locks_detail::make_timepoint(timeout));
    }
#else
    template<class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_until(locks_detail::make_timepoint(timeout));
    }
#endif // FILE_LOCK_USE_BOOST

    /**
     * @brief Tries to aquire exclusive lock until a specified time point
     *
     * @param until time point to try locking until
     * @throws if try_lock() throws
     * @return Returns true if the lock has been aquired
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Clock, class Dur>
    bool try_lock_until(const boost::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, false, until);
    }
#else
    template<class Clock, class Dur>
    bool try_lock_until(const std::chrono::time_point<Clock, Dur>& until)
    {
        return locks_detail::try_lock_obj_until(*this, false, until);
    }
#endif

private:

    /**
     * @brief Try to aquire lock in requested mode
     * @param shared Mode the lock should be requested in
     * @return true if lock was successfully aquired
     */
    bool try_lock(bool shared);

    int m_fd;      ///< the file descriptor used to lock the file
    unsigned m_shared; ///< number of shared locks held
    mutex_t  m_sh_mtx; ///< guard shared locking
};

#endif // !WIN32

/**
 * @brief Wrapper class designed to make NamedFileLock and UnnamedFileLock
 *        thread safe
 *
 * Holds a std::timed_mutex additionally to the LockDevice lock type to make
 * the non thread safe LockDevice thread safe. Calls to the LockDevice type
 * are only done iff the timed_mutex lock is held.
 *
 * @tparam LockDevice The lock device to make thread safe
 */
template<class LockDevice>
class MutexedFileLock
{
public:
#ifdef FILE_LOCK_USE_BOOST
    typedef boost::shared_timed_mutex   mutex_t;
    typedef boost::unique_lock<mutex_t> unique_lock_t;
    typedef boost::shared_lock<mutex_t> shared_lock_t;
    typedef boost::try_to_lock_t        try_to_lock_t;
    typedef boost::defer_lock_t         defer_lock_t;
#else
    using mutex_t       = std::shared_timed_mutex;
    using unique_lock_t = std::unique_lock<mutex_t>;
    using shared_lock_t = std::shared_lock<mutex_t>;
    using try_to_lock_t = std::try_to_lock_t;
    using defer_lock_t  = std::defer_lock_t;
#endif // FILE_LOCK_USE_BOOST

#if (defined(_MSC_VER) && _MSC_VER <= 1700)
    MutexedFileLock(const zf::FileName& file)
        : m_lock_dev(file)
    { }
#else
    /**
     * @brief Construct the MutexedFileLock The lock is not yet taken.
     *
     * Parameters are perfectly forwared to LockDevice constructor
     */
    template<class ... Ts>
    MutexedFileLock(Ts&& ... args)
        : m_lock_dev(std::forward<Ts>(args)...)
    { }
#endif

    /**
     * @brief Take the lock in shared mode
     */
    void lock_shared()
    {
        m_mtx.lock_shared();
        m_lock_dev.lock_shared();
    }

    /**
     * @brief Unlock a previously locked object from shared mode
     */
    void unlock_shared()
    {
        m_mtx.unlock_shared();
        m_lock_dev.unlock_shared();
    }

    /**
     * @brief Tries to take the lock in shared mode
     * @return true if lock taking was successfull
     */
    bool try_lock_shared()
    {
        shared_lock_t lock(m_mtx, try_to_lock_t());
        if (!lock)
            return false;

        if (!m_lock_dev.try_lock_shared())
            return false;

        lock.release();
        return true;
    }

    /**
     * @brief Tries to take the lock in shared mode for specified time
     *
     * @return true if taking the lock was successfull
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Rep, class Period>
    bool try_lock_shared_for(
            const boost::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_shared_until(locks_detail::make_timepoint(timeout));
    }
#else
    template<class Rep, class Period>
    bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_shared_until(locks_detail::make_timepoint(timeout));
    }
#endif

    /**
     * @brief Tries to take the lock in shared mode until specified time
     *
     * @return true if taking the lock was successfull
     */
    template<class TimePoint>
    bool try_lock_shared_until(const TimePoint& until)
    {
        shared_lock_t lock(m_mtx, defer_lock_t());
        if (!lock.try_lock_until(until))
            return false;

        if (!m_lock_dev.try_lock_shared_until(until))
            return false;

        // we obtained both locks in time: keep both
        lock.release();
        return true;
    }

    /**
     * @brief Take the lock
     *
     * Blocks if already locked by an other thread. Locks also LockDevice.
     *
     * @note Calling from the same thread already holding the lock is undefined
     * behaviour and may throw or deadlock.
     */
    void lock()
    {
        m_mtx.lock();
        m_lock_dev.lock();
    }

    /**
     * @brief Release the lock
     *
     * @note Must be locked by the the current thread of execution, otherweise,
     *  the behaviour is undefined.
     */
    void unlock()
    {
        m_lock_dev.unlock();
        m_mtx.unlock();
    }

    /**
     * @brief Tries to lock but doesn't block
     *
     * If true is returned both locks are successfully obtained.
     *
     * @return true if lock has been obtained
     */
    bool try_lock()
    {
        unique_lock_t lock(m_mtx, try_to_lock_t());
        if (!lock)
            return false;

        if (!m_lock_dev.try_lock())
            return false;

        // we obtained both locks now: keep both
        lock.release();
        return true;
    }

    /**
     * @brief Try to lock for a specified duration
     *
     * If the lock could not be obtained within the specified duration, false is
     * returned and no lock is taken.
     * May take longer than the specified duration depending underlying locks
     * implementation or system load.
     *
     * @return true if lock has been obtained. False if timeout was reached
     */
#ifdef FILE_LOCK_USE_BOOST
    template<class Rep, class Period>
    bool try_lock_for(const boost::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_until(locks_detail::make_timepoint(timeout));
    }
#else
    template<class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout)
    {
        return try_lock_until(locks_detail::make_timepoint(timeout));
    }
#endif // FILE_LOCK_USE_BOOST

    /**
     * @brief Just like try_lock_for tries to obtain the lock while blocking
     *   for a specified amount of time.
     *
     * @return true if lock has been obtained. False if timeout was reached
     */
    template<class TimePoint>
    bool try_lock_until(const TimePoint& until)
    {
        unique_lock_t lock(m_mtx, defer_lock_t());
        if (!lock.try_lock_until(until))
            return false;

        if (!m_lock_dev.try_lock_until(until))
            return false;

        // we obtained both locks in time: keep both
        lock.release();
        return true;
    }

private:
    mutex_t m_mtx;         ///< mutex to synchronize access to m_lock_dev
    LockDevice m_lock_dev; ///< the ressource to lock
};


#if (defined(_MSC_VER) && _MSC_VER <= 1700)
typedef MutexedFileLock<NamedFileLock> MutexedNamedFileLock;
#else

#ifndef _WIN32
using MutexedUnnamedFileLock = MutexedFileLock<UnnamedFileLock>;
#endif // _WIN32

using MutexedNamedFileLock = MutexedFileLock<NamedFileLock>;
#endif

} // namespace zfs
#endif