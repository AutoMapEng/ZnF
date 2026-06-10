#pragma once
#include "common_api.h"

#include <memory>
#include <string>
#include <stdint.h>



namespace zfsdll
{
    class FileName;
    class ZFIOFileImpl;

    class COMMON_API ZFIOFile
    {
    public:
        ZFIOFile();
        ZFIOFile(const ZFIOFile& o);
        virtual ~ZFIOFile();

        void copy(const ZFIOFile& o);

        unsigned long getLastErr();

        void flush();

        bool isOpen();

        /**
         * @brief Open file
         *
         * @note File is always opened in binary mode
         *
         * Open flag
         * "r": Open reading, file must exist
         * "w": Create new file
         * "a": Open file if it exist, else it is created. Write only at end
         * "r+": Open file for reading and writing, must exist.
         * "w+": Create new file for reading and writing
         * "a+": Create file if it does not exist. Write only at end
         *
         * @param fn the path to the file to open
         * @param Open flags like fopen()
         * @return true on success
         */
        bool open(const std::wstring& fn, const char* flag);
        bool open(const std::string& fn, const char* flag);

        /**
         * @brief Open file
         *
         * @deprecated This method is deprecated because flags are wrongly
         *          interpreted. Plese use open() instead
         *
         * @param fn path to file to open
         * @param Open flags *not* like fopen() under Windows
         * @return true on success
         */
        bool openFile(const char* fn, const char* flag);
        bool openFile(const wchar_t* fn, const char* flag);
        bool openFile(const std::wstring& fn, const char* flag);
        bool openFile(const FileName& fn, const char* flag);

        /**
         * @brief Flush and close all changes
         */
        void close();

        long tell() const;
        int64_t tell64() const;

        /**
        \return 0 on success, 1 on failure.
        */
        unsigned int seek(int i, int set);

        /**
        \return 0 on success, 1 on failure.
        */
        int64_t seek64(uint64_t i, int set);

        char* gets(char* buf, int maxCount);

        /**
         * @brief Set the new end of file position
         *
         * If the at is behind the current end of file position, the file is
         * extended, otherwise truncated.
         *
         * @param at Postion of the new end of file
         * @return   true on success
         */
        bool setEndOfFile(uint64_t at);

        /// just for windows
#ifdef _WINDOWS
        void* getHandle();
#endif

        /**
        @brief read a line
               a empty line will return true as well
        @return false if end of file
        */
        bool getln(char* buf, int maxCount);

        unsigned long GetFileSize(const wchar_t* fileName);
        int64_t GetFileSize64(const wchar_t* fileName);
        unsigned long GetFileSize() const;

        size_t read(void* text, size_t size, int len);

        size_t write(const void* text, size_t size, size_t len);
        size_t write(const std::string& text) { return write(text.c_str(),1, text.length()); }

    protected:
        std::shared_ptr<ZFIOFileImpl> impl;
    };
} // end namespace zfsdll
