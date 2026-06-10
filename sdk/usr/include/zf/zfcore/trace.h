#pragma once
#include "zfcore_api.h"

#include <functional>
#include <sstream>
#include <string>


namespace zf
{

    /**
     * @file trace.hpp
     *
     * @addtogroup mytools
     * @{
     *
     * @brief Common class based tracing interface
     * @author Karl-Heinz Schneider <k-h.schneider@zofre.de>
     * @date   2015/11/11
     */



#ifdef QT_BEGIN_NAMESPACE
#include <QString>
#endif

     /**
     * @brief Tracer class
     *
     * Can be created with an facility name, which is prepended to each single
     * log message originating from this instance.
     *
     * May only be an interface. Application needs to provide it's own
     * implementation / redirection.
     */
    class ZFCORE_API Tracer
    {
    public:
        /**
         * Tracing severities in ascending order
         * Level::log is special:
         *  -> corresponds to Level::info but is also put into logfile if it exists
         */
        enum Levels { debug, info, warn, error, fatal, log };

        /**
         * @brief Creates an Logger instance with empty facility name
         */
        Tracer()
        { }

        /**
         * @brief Creates an Logger with given facility name
         * @param facility Name of the facility
         */
        Tracer(const std::string& facility)
            : _facility(facility)
        { }

        /**
         * @brief Create a log message
         *
         * New line caracter will be automatically appended to the log message.
         *
         * @param level severity of the log message
         * @param msg   Message body
         */
        void trace(Levels level, const char* msg)
        {
            trace(level, std::string(msg));
        }

        /** @overload void trace(Levels level, const char* msg) */
        void trace(Levels level, const wchar_t* msg)
        {
            trace(level, std::wstring(msg));
        }

        /** @overload void trace(Levels level, const char* msg) */
        void trace(Levels level, const std::string& msg);

        /** @overload void trace(Levels level, const char* msg) */
        void trace(Levels level, const std::wstring& msg);

        // don't force dependency to Qt
#ifdef QT_BEGIN_NAMESPACE
    /** @overload void trace(Levels level, const char* msg); */
        void trace(Levels level, const QString& msg)
        {
            trace(level, msg.toStdString());
        }
#endif

        // don't force dependency to boost
#ifdef BOOST_FORMAT_CLASS_HPP
        void trace(Levels level, const boost::format& formated)
        {
            trace(level, boost::str(formated));
        }
#endif

        /**
         * @brief Create an debug message
         * @param msg Message body
         */
        template<class T>
        void tdebug(const T& msg) { trace(Tracer::debug, msg); }

        /**
         * @brief Create an info message
         * @param msg Message body
         */
        template<class T>
        void tinfo(const T& msg) { trace(Tracer::info, msg); }

        /**
         * @brief Create an warning message
         * @param msg Message body
         */
        template<class T>
        void twarn(const T& msg) { trace(Tracer::warn, msg); }

        /**
         * @brief Create an error message
         * @param msg Message body
         */
        template<class T>
        void terror(const T& msg) { trace(Tracer::error, msg); }

        /**
         * @brief Create an fatal message
         * @param msg Message body
         */
        template<class T>
        void tfatal(const T& msg) { trace(Tracer::fatal, msg); }

        /**
         * @brief Create an info message which also goes into log file
         * @param msg Message body
         */
        template<class T>
        void tlog(const T& msg) { trace(Tracer::log, msg); }

        //
        // streams interface for tracing
        //

        /**
         * @brief A sigle trace/log message
         *
         * Message gets committed at destruction time
         */
        class message : public std::ostringstream
        {
            Tracer& m_self;
            Levels  m_level;

        public:
            message(Tracer& t, Levels l)
                : m_self{ t }
                , m_level{ l }
            { }

            message(message&& other)
                : std::ostringstream{ std::move(other) }
                , m_self{ other.m_self }
                , m_level{ other.m_level }
            {}

            /**
             * @brief Commit message
             */
            ~message() override { m_self.trace(m_level, str()); }
        };

        /**
         * @brief Prepare a new log message
         * @param l Logging level of the newly created message
         * @return log message
         */
        message make_message(Levels l) { return message(*this, l); }

        /**
         * @brief Produce a new debugging message
         * @return log message
         */
        message tdebug() { return make_message(Tracer::debug); }

        /**
         * @brief Produce a new info message
         * @return log message
         */
        message tinfo() { return make_message(Tracer::info); }

        /**
         * @brief Produce a new warning message
         * @return log message
         */
        message twarn() { return make_message(Tracer::warn); }

        /**
         * @brief Produce a new error message
         * @return log message
         */
        message terror() { return make_message(Tracer::error); }

        /**
         * @brief Produce a new fatal message
         * @return log message
         */
        message tfatal() { return make_message(Tracer::fatal); }

        /**
         * @brief Produce a new log message
         * @return log message
         */
        message tlog() { return make_message(Tracer::log); }

    private:
        const std::string _facility;  ///< Name of the traced module/code
    };


    namespace tracecfg
    {
        typedef std::function<
            void(Tracer::Levels, const std::string& fac, const std::string& msg)>
            str_cb_func;
        typedef std::function<
            void(Tracer::Levels, const std::wstring& fac, const std::wstring& msg)>
            wstr_cb_func;

        /**
         * @brief Function to register a callback called if a trace message
         *        is produced.
         *
         * There exist 2 variants: One for std::string and one for std::wstring.
         * If only one callback is registerd, the messages are converted to the
         * other (utf-8 -> wchar_t and vice versa) using internal functions.
         *
         * You may register both callbacks to to the conversion yourself.
         *
         * If called, the trace output is no longer written to std::cout!
         *
         * @param sf  Callback for char based strings (utf-8)
         * @param wsf Callback for wchar_t based strings
         */
        void ZFCORE_API register_cb(const str_cb_func& sf, const wstr_cb_func& wsf);

        /**
         * @brief Return the callback functions currently registered
         *
         * @return two functions for outputing trace messages
         */
        std::tuple<str_cb_func, wstr_cb_func> get_cb_functions();

        /**
         * @brief Reset trace callbacks. Output is written to std::cerr
         */
        void ZFCORE_API reset_cb();
    } // namespace trace
    /** @} */

}
