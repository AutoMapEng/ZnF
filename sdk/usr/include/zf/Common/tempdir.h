#pragma once

#include <string>

namespace zfsdll {

/**
 * @brief Creates a random directory in a system tmp directory
 *
 * If the Object runs out of scope, all files and the directory itself is
 * removed again.
 *
 * The directory is created below the system temporary directory with following
 * pattern:
 *
 * $tempdir/$basenameX
 *
 * where X is replaced with a random string to avoid collisions
 *
 * Tempdir handles a resource and is therefore non copyable!
 */
class Tempdir
{
public:
    /**
     * @brief Create a temporary directory without prefix
     * @throws std::system_error on failure
     */
    Tempdir();

    /**
     * @brief Create a temporary directory using a prefix
     * @param basename Prefix to use
     * @throws std::system_error on failure
     */
    explicit Tempdir(const std::string& basename);

    Tempdir(const Tempdir&) = delete;
    Tempdir& operator=(const Tempdir&) = delete;

    Tempdir(Tempdir&&);
    Tempdir& operator=(Tempdir&&) noexcept;

    /**
     * @brief Remove created temporary directory recursively
     */
    ~Tempdir() noexcept;

    /**
     * @brief The path to the created directory
     * @return path to directory
     */
    const std::string& dirname() const
    {
        return m_dirname;
    }

private:
    std::string m_dirname; ///< path to temporary directory
};

} // end namespace zfsdll
