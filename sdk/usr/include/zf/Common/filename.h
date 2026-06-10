#pragma once
#include "common_api.h"

#include <string>
#include <vector>

namespace zfsdll
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
    class COMMON_API FileName
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
        FileName(const std::wstring& path, const std::wstring& file, const std::wstring& ext=L"" );   ///< build a abs filename. Add extension if not already defined

        void clear();

        FileName& Set(const FileName& s);
        FileName& fromCodepage(const char* s);           // Codepage
        FileName& fromUtf8(const std::string& s);    // UTF8
        FileName& fromWChar(const wchar_t* s);        // UTF16
        FileName& fromUtf16(const std::wstring& s);   // UTF16

        FileName& Set(const char* s) { return fromCodepage(s);  }
        FileName& Set(const std::string& s) { return fromUtf8(s);  }
        FileName& Set(const std::wstring& w) { return fromUtf16(w);  }
        FileName& Set(const wchar_t* w) { return fromWChar(w); }


        bool RemoveExt();
        bool RemoveExt(const char* ext);
        bool RemoveExt(const std::string& ext);
        bool RemoveExt(const wchar_t* ext);
        bool RemoveExt(const std::wstring& ext);
        bool RemoveExt(const std::vector<std::wstring>& extensions);
        bool RemoveExtFull(std::wstring& foundextension); //fileextension is deleted until. //TODO eventuell loeschen
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
        FileName& replacePath(const std::wstring& newPath ); ///< replace the whole path and keep the filename
        FileName& replacePath(const FileName& ); ///< replace the whole path and keep the filename
        bool RemoveName();
        bool RemovePath();
        FileName& removeName();
        FileName& removePath();
        bool AddPath(const char* p);
        bool AddPath(const std::string& p);
        bool AddPath(const wchar_t* p);
        bool AddPath(const std::wstring& p);
        FileName& addPath(const std::wstring& );
        FileName& AddName(const char* p);
        FileName& AddName(const std::string& n);
        FileName& AddName(const wchar_t* p);
        FileName& AddName(const std::wstring& n);
        FileName& AddExt(const wchar_t* ex);
        FileName& AddExt(const std::wstring& ex);
        FileName& AddExt(const std::string& ex);
        FileName& AddExt(const char* ex);
        const char* getExtC();
        const wchar_t* getExtW();
        FileName& InsertPath(const char* path, bool createFolder=false); // Filename must be fullfilename
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
        bool IsAbs() const; // true if absolute path
        bool Exist() const;
        bool Delete(); // DeleteFile
        FileName& Normalize(); /// ruft Slash auf und entfernt doppelte //

        int length() const { return static_cast<int>(fn.length()); }

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
        const std::string& String()  const;
        const wchar_t* w_str() const { return fn.c_str(); }
        const char* c_str() const; // obsolete

        std::string utf8() const;
        const std::wstring& utf16() const { return fn; }
#ifdef _WINDOWS
        std::string codepage() const;
#endif

#ifdef _WINDOWS
        const std::wstring& ToNative() const
        {
            return WString();
        }
#else
        const std::string& ToNative() const
        {
            return String();
        }
#endif

        FileName& slash();

        void SetDrive(const char* d, bool force = false);
        void SetDrive(const wchar_t* d, bool force = false);
        std::wstring GetDrive();
        bool HasDrive() const;

        // Überfluessige Leerzeichen entfernen
        const wchar_t* trim();

        // file name ends with the extension
        bool endsWith(const wchar_t* ext) const;
        bool endsWith(const std::wstring& ext) const;
        bool endsWith(const char* ext) const;
        bool endsWith(const std::string& ext) const;
        /// file name ends with one of the extensions in the list
        bool endsWithOneOf(const std::vector<std::wstring>& exts) const;

        bool startsWith(const std::wstring &str) const;

        int icmp(const char* s) const;
        int icmp(std::string& s) const;
        int icmp(const wchar_t* s) const;
        int icmp(std::wstring& s) const;

        const char* ToShortPathName(bool*ok = nullptr);

        static bool endsWith(const std::wstring& s, const std::wstring& ext);
        static bool endsWith(const std::wstring& s, const std::string& ext);
        static bool endsWith(const std::string& s, const std::string& ext);

        static int rename(const char* oldName, const char* newName);
        static int rename(const std::string& oldName, const std::string& newName);
        static int rename(const wchar_t* oldName, const wchar_t* newName);
        static int rename(const std::wstring& oldName, const std::wstring& newName);

        //Moeglichst nicht für scannamen verwenden
        void makeLower(); // should not be called

    private:
        bool caseInsensitiveWStringCompare(const std::wstring& a, const std::wstring& b) const;
    };

#ifndef __GNUG__
# pragma warning( pop )
#endif
} // end namespace zfsdll
