#pragma once
#include "zftools_api.h"
#include <zfcore/compiler.h>

#include <string>
#include <mutex>
#include <memory>
#include <functional>


namespace zf
{
#define MAX_LOG_PRINT_LEN   1024


    /**
    * trace or log messages
    * depending on level, just errors or all is reported
    * support a module name as well
    * log can be redirected to a different logger as well
    * logoutput will start with time, level and module followed by the message
    * support to write to a file
    */
    class ZFTOOLS_API Log
    {
    public:
        Log() = default;
        Log( std::shared_ptr<Log> );
        Log(const std::wstring& file );
        Log(const std::string& modul);

        enum level_t
        {
            LOG_OFF,
            LOG_ERROR, // log just errors
            LOG_WARNING,
            LOG_TRACE,

            _LOG_
        };

        /// redirect to a different logger
        void redirect(std::shared_ptr<Log> l);


        /// print time at start of line
        void setWithTime(bool en) { m_withTime = en; }
        
        /// with the modulename if defined
        void setWithModulename(bool en) { m_withModulename = en; }

        /// output the logging to console as well
        void PrintToConsole(bool print) { m_printToConsoleAsWell = print; }

        void SetLogFileName(const std::wstring& fn, bool clearFile = false );
        std::wstring LogFileName() const { return m_fn; }

        /// clear logging file. Filename has to be set before
        void ClearFile();


        level_t getLogLevel() const { return m_logLevel; }
        void setLogLevel(level_t l) { if (l < LOG_OFF) m_logLevel = LOG_OFF; else if (l >= _LOG_) m_logLevel = static_cast<level_t>(_LOG_-1); else m_logLevel = l; }
        void setLogLevel(int l) { setLogLevel( static_cast<level_t>(l) ); }
        bool IsEnabled() const { return m_logLevel != LOG_OFF; }

        void enableAll() { m_logLevel = LOG_TRACE; }
        void enableErrors() { m_logLevel = LOG_ERROR; }
        void disable() { m_logLevel = LOG_OFF; }



        /// \n will be added automatically
        void print(level_t, const std::string&, const std::string& modulName="" );
        void print(level_t, const std::wstring&, const std::string& modulName = "");
        void printf(level_t, const char*, ...);

        /// peset level
        void trace(const std::string& s) { print(LOG_TRACE, s); }
        void trace(const std::wstring& s) { print(LOG_TRACE, s); }
        void warning(const std::string& s) { print(LOG_WARNING, s); }
        void warning(const std::wstring& s) { print(LOG_WARNING, s); }
        void error(const std::string& s) { print(LOG_ERROR, s); }
        void error(const std::wstring& s) { print(LOG_ERROR, s); }


        typedef std::function< void(const std::string&) > print_cb;
        void setPrintCB(print_cb _cb);

    protected:
        std::string m_modul{};
        level_t m_logLevel{ LOG_OFF };
        std::wstring m_fn;
        bool m_printToConsoleAsWell{};
        std::mutex m_mut{};
        bool m_withTime{ false };
        bool m_withModulename{ false };
        std::shared_ptr<Log> m_redirect{};
        print_cb cb{};
    };
}
