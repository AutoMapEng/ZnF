#pragma once
#include "zfcore_api.h"

#include <memory>
#include <string>



namespace zf
{
    class FileName;
    class ZFFileImpl;

    class ZFCORE_API ZFFile
    {
    public:
        ZFFile();
        ZFFile(const ZFFile& o);
        virtual ~ZFFile();

        void copy(const ZFFile& o);

        unsigned long getLastErr() const;

        void flush();

        bool isOpen() const;

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

        bool create(const std::wstring& fn) { return open(fn, "w"); }
        bool create(const std::string& fn) { return open(fn, "w"); }

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
        //bool openFile(const char* fn, const char* flag);
        //bool openFile(const wchar_t* fn, const char* flag);
        //bool openFile(const std::wstring& fn, const char* flag);
        //bool openFile(const FileName& fn, const char* flag);

        /**
         * @brief Flush and close all changes
         */
        void close();

        int64_t tell() const;

        /**
        \return 0 on success, 1 on failure.
        */
        int64_t seek(int64_t i, int set);

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
        void* getHandle() const;
#endif

        /**
        @brief read a line
               a empty line will return true as well
        @return false if end of file
        */
        bool getln(char* buf, int maxCount);

        /// filesize of any file. return 0 if file does not exist
        static int64_t GetFileSize(const std::wstring& fileName);

        // filesize of current file
        int64_t GetFileSize() const;

        size_t read(void* text, size_t size, int len);

        size_t write(const void* text, size_t size, size_t len);
        size_t write(const std::string& text) { return write(text.c_str(),1, text.length()); }

    protected:
        std::shared_ptr<ZFFileImpl> impl;
    };
} // end namespace zf
