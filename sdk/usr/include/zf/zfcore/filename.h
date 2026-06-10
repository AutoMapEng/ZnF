#pragma once
#include "zfcore_api.h"

#include <string>
#include <vector>


namespace zf
{
#ifndef __GNUG__
# pragma warning( push )
# pragma warning( disable : 4251 )
#endif

    /**
    \brief Class for manipulating filenames

    filename is stored internaly as utf16

    Windows
    const char*    codepage version of the filename
    std::string    UTF8 version of the filename
    std::wstring   UTF16 version of the filename
    Linux
    const char*    UTF8
    std::string    UTF8
    std::wstring   UTF16
    */
    class ZFCORE_API FileName
    {
        std::wstring fn;        // UTF16

        std::wstring retValUTF16;
        std::string retValCP;

    public:
        FileName() {}
        FileName(const char* f);          // CP
        FileName(const std::string& f);   // CP
        FileName(const wchar_t* f);       // UTF16
        FileName(const std::wstring& f);   // UTF16
        FileName(const std::wstring& path, const std::wstring& file, const std::wstring& ext = L"");   ///< build a abs filename. Add extension if not already defined

        void clear();

        FileName& fromCodepage(const char* s);           // Codepage
        FileName& fromUtf8(const std::string& s);    // UTF8
        FileName& fromWChar(const wchar_t* s);        // UTF16
        FileName& fromUtf16(const std::wstring& s);   // UTF16

        FileName& Set(const FileName& s);
        FileName& Set(const char* s) { return fromCodepage(s); }
        FileName& Set(const std::string& s) { return fromUtf8(s); }
        FileName& Set(const std::wstring& w) { return fromUtf16(w); }
        FileName& Set(const wchar_t* w) { return fromWChar(w); }


        bool RemoveExt();
        bool RemoveExt(const char* ext);
        bool RemoveExt(const std::string& ext);
        bool RemoveExt(const wchar_t* ext);
        bool RemoveExt(const std::wstring& ext);
        bool RemoveExt(const std::vector<std::wstring>& extensions);
        bool RemoveExtFull(); //fileextension is deleted until. //TODO eventuell loeschen
        FileName& removeExt(); ///< remove any kind of extension
        FileName& removeExt(const std::vector<std::string>& extensions); ///< remove one of the extension in the lis
        FileName& removeExtW(const std::vector<std::wstring>& extensions);
        void ReplaceExt(const wchar_t* ext);
        void ReplaceExt(const std::wstring& ext);
        void ReplaceExt(const char* ext);
        void ReplaceExt(const std::string& ext);
        FileName& ReplaceExt(const std::wstring& extToRemove, const std::wstring& extToAdd);
        FileName& ReplaceExt(const std::string& extToRemove, const std::string& extToAdd);
        FileName& replaceExtW(const std::vector<std::wstring>& extsRemove, const std::wstring& wextAdd);
        FileName& replaceExt(const std::vector<std::string>& extsToRemove, const std::string& extToAdd);
        FileName& replacePath(const std::wstring& newPath); ///< replace the whole path and keep the filename
        FileName& replacePath(const FileName&); ///< replace the whole path and keep the filename
        bool RemoveName();
        bool RemovePath();
        FileName& removeName();
        FileName& removePath();
        bool AddPath(const char* p);
        bool AddPath(const std::string& p);
        bool AddPath(const wchar_t* p);
        bool AddPath(const std::wstring& p);
        FileName& addPath(const std::wstring&);
        FileName& AddName(const char* p);
        FileName& AddName(const std::string& n);
        FileName& AddName(const wchar_t* p);
        FileName& AddName(const std::wstring& n);
        FileName& AddExt(const wchar_t* ex);
        FileName& AddExt(const std::wstring& ex);
        FileName& AddExt(const std::string& ex);
        FileName& AddExt(const char* ex);
        const std::string getExtC() const;
        const std::wstring getExtW() const;
        FileName& InsertPath(const char* path, bool createFolder = false); // Filename must be fullfilename
        FileName& InsertPath(const std::string& path, bool createFolder = false); // Filename must be fullfilename
        FileName& InsertPath(const wchar_t* path, bool createFolder = false); // Filename must be fullfilename
        FileName& InsertPath(const std::wstring& path, bool createFolder = false); // Filename must be fullfilename
        bool MkDir();
        FileName& mkDir();
        bool MkDirWithoutName(); // verzeichniss erstellen wobei voller dateiname uebergeben wurde
        FileName& mkDirWithoutName(); // verzeichniss erstellen wobei voller dateiname uebergeben wurde
        FileName& RelPath(const char* rootpath);
        FileName& RelPath(const std::string& rootpath);
        FileName& RelPath(const wchar_t* rootpath);
        FileName& RelPath(const std::wstring& rootpath); // remove top root path

        /// Filename should be a folder
        bool isSubfolderOf( const std::wstring& pathCompareTo ) const;

        /// true if absolute path
        bool IsAbs() const; 

        /// check if folder or file exists
        bool Exist() const;

        bool Delete(); // DeleteFile
        bool DeleteDirectory(const std::wstring& extension = {}); ///< delete all files in folder but not subfolders. Have to be a folder. All files or just with extension like L".jpg"
        bool DeleteDirectoryRecursive(); //< delete all files in folder as well as all files in subfolders and subfolder as well. Have to be a folder
        bool DeleteEmptyDirectories();

        bool isDirectory() const;
        bool isRegularFile() const;
        
        /**
         * \brief check if a file is read only
         * 
         * \return true if it is a regular file, exists and can be written
         */
        bool isFileWritable() const;

        /**
         * \brief Checks if directory exists is a directory and can be written to
         * 
         * \return true if writable or false if not or is not a directory or does not exist
         */
        bool isDirectoryWritable() const;

        FileName& Normalize(); /// ruft Slash auf und entfernt doppelte //

        /**
        *   True if the time of the given Object is greater than the time of the Object
        *   \param pathName => Check Object Must contain the complete path and names.
        *   \param typ => 1 = CreationTime 2 = LastAccessTime 3 = LastWriteTime
        *   \return     1 = This file time is later than given file time.
        *               0 = This file time is equal to given file time. 
        *              -1 = This file time is earlier than given file time. 
        *              -3 = Error maybe wrong typ
        */
        enum class access_t : int { CREATION_TIME=1, LAST_ACCESS_TIME=2, LAST_WRITE_TIME=3 };
        enum class compare_t : int { LATER=1, EQUAL=0, EARLIER=-1, FAILED=-3 };
        compare_t timeIsGreaterThan(const std::wstring& pathName, access_t typ) const; 

        // length of filepath
        int length() const { return static_cast<int>(fn.length()); }
        int size() const { return length(); }
        bool empty() const { return size() == 0; }

        const char* Replace(const std::string& rep, const std::string& wit);
        const wchar_t* Replace(const std::wstring& rep, const std::wstring& wit);

        void operator= (const char* n);
        void operator= (const wchar_t* n);
        void operator= (const std::string& n);
        void operator= (const std::wstring& n);
        void operator+= (const char* n);
        void operator+= (const wchar_t* n);
        void operator+= (const std::string& n);
        void operator+= (const std::wstring& n);
        void operator+= (const FileName& n);
        // comparison is case insensitive
        bool operator== (const char* n) const;
        bool operator== (const std::string& n) const;
        bool operator== (const wchar_t* n) const;
        bool operator== (const std::wstring& n) const;
        bool operator== (const FileName& n) const;
        bool operator!= (const char* n) const;
        bool operator!= (const std::string& n) const;
        bool operator!= (const wchar_t* n) const;
        bool operator!= (const std::wstring& n) const;
        bool operator!= (const FileName& n) const;

        const std::wstring& WString() const { return fn; }
        const std::string String()  const;
        const wchar_t* w_str() const { return fn.c_str(); }
        const char* c_str() const; // obsolete

        std::string utf8() const;
        const std::wstring& utf16() const { return fn; }
#ifdef _WINDOWS
        std::string codepage() const;
#endif

#ifdef _WINDOWS
        const std::wstring ToNative() const
        {
            return WString();
        }
#else
        const std::string ToNative() const
        {
            return String();
        }
#endif

        FileName& slash();

        void SetDrive(const char* d, bool force = false);
        void SetDrive(const wchar_t* d, bool force = false);
        std::wstring GetDrive();
        bool HasDrive() const;

        // �berfluessige Leerzeichen entfernen
        const wchar_t* trim();

        // file name ends with the extension
        bool endsWith(const wchar_t* ext) const;
        bool endsWith(const std::wstring& ext) const;
        bool endsWith(const char* ext) const;
        bool endsWith(const std::string& ext) const;
        /// file name ends with one of the extensions in the list
        bool endsWithOneOf(const std::vector<std::wstring>& exts) const;

        bool startsWith(const std::wstring& str) const;

        // compare
        int icmp(const char* s) const;
        int icmp(std::string& s) const;
        int icmp(const wchar_t* s) const;
        int icmp(std::wstring& s) const;

        const std::string ToShortPathName(bool* ok = nullptr);

        static bool endsWith(const std::wstring& s, const std::wstring& ext);
        static bool endsWith(const std::wstring& s, const std::string& ext);
        static bool endsWith(const std::string& s, const std::string& ext);

        static int rename(const char* oldName, const char* newName);
        static int rename(const std::string& oldName, const std::string& newName);
        static int rename(const wchar_t* oldName, const wchar_t* newName);
        static int rename(const std::wstring& oldName, const std::wstring& newName);

        //Moeglichst nicht f�r scannamen verwenden
        void makeLower(); // should not be called

        /**
         * list files in current folder
         * 
         * \param extension can be empty for list all files
         * \param subdirs go to subdirs
         * \return list of files
         */
        std::vector< std::wstring > listFiles(const std::wstring& extension = {}, bool subdirs = false) const;

        /**
         * List files like  <basefilename>.anythinf.<ext>
         * 
         * \param basefilename without extension
         * \param extension
         * \param subdirs
         * \return list of files
         */
        std::vector< std::wstring > listFilesBase(const std::wstring& basefilename, const std::wstring& extension, bool subdirs = false) const;

        /// filename must be a path. Maybe call removename before
        std::vector< std::wstring > listFolders() const;

#ifdef _WINDOWS
        /// copy a file to dest
        bool copy(const std::wstring& dest, bool overwriteIfExist = true);
        bool copyFolder(const std::wstring& dest, bool overwriteIfExist = true);
#endif

    private:
        bool caseInsensitiveWStringCompare(const std::wstring& a, const std::wstring& b) const;
    };

#ifndef __GNUG__
# pragma warning( pop )
#endif

}
