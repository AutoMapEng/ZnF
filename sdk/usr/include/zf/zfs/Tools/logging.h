#pragma once

#include "../zfsapi.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace zfs
{
#if 0
    typedef enum LogTypeSet { LT_STANDARD, LT_STATUS, LT_RESULT, LT_PARAMETER } LogType;

    class ZFS_API logging
    {
    public:

        logging();

        logging( const std::wstring& aDatasetPath, const std::string& aModulName, const LogTypeSet& aLoggingLevel, const bool& abEnable= true);
        logging( const std::wstring& aDatasetPath, const std::string& aModulName, const int aLogFileNumber);

        ~logging();

        void init(const std::wstring& aDatasetPath, const std::string& aModulName, const LogTypeSet& aLogType, const bool& abEnable= true);
        void init(const std::wstring& aDatasetPath, const std::string& aModulName, const int aLogFileNumber);

        logging& operator<<( const LogType& aLogType );
        logging& operator<<( const char* aElement );

        template<class T> logging& operator<<( const T& aElement );
        
        void setLoggingLevel(const LogTypeSet& aLogType);

        void enable(const bool& flag= true);
        void disable();

        bool enabled() const;
        
    private:

        bool                      m_LoggingEnabled;
        bool                      m_LoggingTypeSet;
                                  
        std::wstring              m_LogFilePath;
        std::string               m_ProcessedFilePath;
                                  
        std::string               m_ModuleName;

        std::string               m_Logging_Prefix;

        std::vector<std::string>  m_Logging_Types;

        std::stringstream         m_LoggingStream;
                                  
        std::ofstream             m_LoggingOut;

        LogTypeSet                m_CurrentLoggingType;
        LogTypeSet                m_CurrentLoggingLevel;

        void base_init(const std::string& aModuleName);
        void reset(const std::string& aModuleName);
        void init_logging_types();

        bool openLogFile(const std::wstring& aLogFileName);
        void writeLogStartLine();
        void writeToLogFile();
        void closeLogFile();

        void setLogFileName(const std::wstring& aDatasetName, const std::string& aModulName);
        void setLogFileName(const std::wstring& aDatasetName, const std::string& aModulName, const int aLogFileNumber);

    };

    // ===============================================================================================================================
    // Useful typedefs
    // ===============================================================================================================================
    typedef logging *LoggingPtrType, * const ConstLoggingPtrType;
    
    // ===============================================================================================================================
#endif

}