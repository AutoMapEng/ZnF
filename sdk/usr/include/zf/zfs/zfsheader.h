#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

//#include <mutex> // this is not allowed because otherwise no common language support is possible

#include "zfsapi.h"
#include "zfserr.h"

#include <Common/typ.h>


namespace std
{
    class mutex;
}

/*
File-Format:

Address
0..2    filemarking "ZFS"
3..4    version of fileformat (WORD i.e. 0201h = 2.1 )
5..6    address of start of data = headersize (WORD i.e. 4096d)
7	    reserved (BYTE = 0)
8..	    Start of items (see below)


Items (each one has at least 4DWORD's = 16Byte)
0..1	index (see below)
2	    len of value (4=DWORD, 8=double, 9=two floats, or stringlength if type=1)
3	    type (0=solid value, 1=string/array ) from fileversion 2.1
4..15	value if it is not a string/array
...     maybe more if string/array used. Setting ends always at a boundary of 16Bytes. Sinze fileversion 2.1

last item has index 0xffff

*/



#pragma pack(1) // struct byte-width
class ZFS_API ZFSHeaderItem
{
protected:
    friend class ZFSHeader;

    uint16_t index;
    uint8_t  len;    // len = len of value
    uint8_t  typ;    // 0=long/double 1=array[len=length]
    union
    {
        struct
        {
            uint32_t l;
            uint32_t h;
        }dw;
        struct
        {
            float f1;
            float f2;
        }f;
        double d;
        char b;
    }u;
    uint32_t dummy;

public:
    enum type_t { TYPE_UNKNOWN, TYPE_LONG, TYPE_DOUBLE, TYPE_FLOAT2, TYPE_STRING }; ///< Type of item
    enum { LEN_DWORD=4, LEN_DOUBLE=8, LEN_FLOAT2=9 };

    ZFSHeaderItem();
    ZFSHeaderItem(uint16_t idx, uint32_t dw)        { typ=0; dummy=0; index=idx; len=4; u.dw.l=dw; u.dw.h=0u; }
    ZFSHeaderItem(uint16_t idx, double dbl)         { typ=0; dummy=0; index=idx; len=8; u.d=dbl; }
    ZFSHeaderItem(uint16_t idx, float f1, float f2) { typ=0; dummy=0; index=idx; len=9; u.f.f1=f1; u.f.f2=f2; }
    ZFSHeaderItem(uint16_t idx, uint8_t dw)         { typ=0; dummy=0; index=idx; len=4; u.dw.l=static_cast<uint32_t>(dw); u.dw.h=0u; }
    ZFSHeaderItem(uint16_t idx, uint16_t dw)        { typ=0; dummy=0; index=idx; len=4; u.dw.l=static_cast<uint32_t>(dw); u.dw.h=0u; }
    ZFSHeaderItem(uint16_t idx, bool dw)            { typ=0; dummy=0; index=idx; len=4; u.dw.l=static_cast<uint32_t>(dw); u.dw.h=0u; }
    ZFSHeaderItem(uint16_t idx, int32_t dw)         { typ=0; dummy=0; index=idx; len=4; u.dw.l=static_cast<uint32_t>(dw); u.dw.h=0u; }
    ZFSHeaderItem(uint16_t idx, const char* str);

    uint16_t Index() const { return index; }
    type_t   Type()  const;
    uint8_t  Len()   const { return len; }
    uint8_t  Typ()   const { return typ; }

    int Size()       const { return typ==0 ? SizeStatic() : SizeStr(); }
    int SizeStatic() const { return 16; }
    int SizeStr()    const { return 16 * (len/16 + ((len % 16) ? 1:0) ) + SizeStatic(); }// allways with a boundery of 16

    void Set(uint16_t idx, double dbl)          { typ=0; index=idx; len=8; u.d=dbl; }
    void Set(uint16_t idx, float f1, float f2 ) { typ=0; index=idx; len=9; u.f.f1=f1; u.f.f2=f2; }
    void Set(uint16_t idx, uint32_t d)          { typ=0; index=idx; len=4; u.dw.l=d; u.dw.h=0u; }
    void Set(uint16_t idx, int32_t w)           { Set(idx, static_cast<uint32_t>(w)); }
    void Set(uint16_t idx, uint8_t w)           { Set(idx, static_cast<uint32_t>(w)); }
    void Set(uint16_t idx, uint16_t w)          { Set(idx, static_cast<uint32_t>(w)); }
    void Set(uint16_t idx, bool w)              { Set(idx, static_cast<uint32_t>(w)); }
    void Set(uint16_t idx, const char* str);

    /// return the double
    double    Dbl()   const;
    /// return first float value
    float     Float1()const;
    /// return second float value
    float     Float2()const;
    /// return the 32bit unsigned
    uint32_t  DWord() const;
    /// return the bool
    bool      Bool()  const;
    /// return the 16bit unsigned
    uint16_t  Word()  const;
    /// return the 8bit unsigned
    uint8_t   Byte()  const;
    /// return the string
    const char* Str() const;

    /// return item as a string independent from the type
    std::string asString() const;
    bool setViaString(const std::string&);

private:
    bool isVisible() const;
};
#pragma pack()

namespace zfs { namespace local { class ZFSHeaderLoc; } }



class ZFS_API ZFSHeader
{
    friend class zfs::local::ZFSHeaderLoc;

#pragma pack(1) // byte align
    struct SHead // id of file
    {
        char      kennung[3];   // "ZFS"
        uint16_t  version;      // MSB=major LSB=minor
        uint16_t  datastart;    // start of scandata
        char      reserved;     // = 0
    }head;
    std::vector<char> pParam;     // buffer for header
#pragma pack()

    std::wstring      mFileName;
    ZFSHeaderItem* ptr; // used by First/Next
    int posZFS; // position of ZFS signatur (Read/WriteEx)
    bool modified; ///< true wheneven a value is set, reset while load and save
    int mode{};

    mutable std::shared_ptr<std::mutex> rwMutex;

    void Destroy();

    bool mUseCallback;

public:
    ZFSHeader();
    virtual ~ZFSHeader();

    /// teh default header size if not other spezified
    static int defaultHeaderSize();

    /// the maximum size the header could be
    static int maxHeaderSize();

    /// returns true if header was created and has a defined size
    bool IsValid() const;

    /// set a filename
    void setFilename( const std::wstring& ); 

    /// return the filename if set
    const wchar_t* Filename() const { return mFileName.c_str(); }

    /// read header from file
    bool Read( const std::wstring& filename );
    bool Read( const std::string& filename );
    zfs::zfserr_t ReadE( const std::wstring& filename );

    /// write buffer back to file. file must exist, be closed or must allow sharing write
    bool Write();                       
    bool WriteEx( const std::wstring& filename=L""); // write buffer back to file. file must exist, be closed or must allow sharing write
    bool Write( const std::wstring& filename ); // write buffer to file.
    
    /// write buffer to file. Creates a empty file with a new header
    bool WriteCreate( const std::wstring& filename ); 

    /// resize the header. maximum = maxHeaderSize()
    bool Resize( int newsize );

    /// resize a header of a existing file on disk
    bool Resize( int newsize, const std::wstring& filename );

    bool Copy( const ZFSHeader* from );
    bool CopyFromBuffer( const char* ); /// create from a memory buffer. Must start with "ZFS"
    bool CopyFromBuffer(const char*, int sz); 
    bool CopyFromBuffer(const std::vector<char>& ); 

    /// return memory image as it will be stored in the file
    std::vector<char> GetAsMemoryImage() const; 

    int  pos() const    { return posZFS; }
    void setPos( int p) { posZFS=p; }

    unsigned short Version() const  { return head.version; }    // version of the header
    void CreateEmptyHeader( int size=0 );   // create a buffer with a empty header 0=default-header-size
    int  HeaderSize() const         {return head.datastart;}    // size of the whole header
    int  ParamSize()  const         {return head.datastart-sizeof(SHead);}
    int  HeadSize()   const         {return sizeof(SHead);} // sizeof head
    void Usage( int& n, int& bytesused, int& bytesmax ) const;
    int  GetFreeItems() const; ///< returns free items
    int  items() const; ///< number of items in the header

    /// save new value to item or adds new if not exist
    bool AddItem( const ZFSHeaderItem& var, const char* data=NULL ); 
    bool SetItem( const ZFSHeaderItem& var, const char* data=NULL ) { return AddItem(var,data); } // save new value to item or adds new if not exist
    
    /// find item in header and return a pointer to it. Null if item not found
    const ZFSHeaderItem* Find( int idx ) const;
    ZFSHeaderItem* Find( int idx );     ///< find item in header and return a pointer to it. Null if item not found

    // remove a item. returns true if item was found and removed
    bool Remove( int idx ); 

    /// access the items
    bool isVisible( int idx ) const;

    double GetDouble(int idx, double def) const;     // find item in header and return value if exist otherwise return default value
    int32_t GetLong(int idx, int32_t def) const;     // find item in header and return value if exist otherwise return default value
    uint32_t GetULong(int idx, uint32_t def) const;     // find item in header and return value if exist otherwise return default value
    const char* GetStringC( int idx, std::string& def ) const;     // find item in header and return value if exist otherwise return default value returns utf8 coded string
    std::string GetStringC( int idx, const std::string& def ) const; // find item in header and return value if exist otherwise return default value returns utf8 coded string
    const wchar_t* GetStringW( int idx, std::wstring& def ) const;
    std::wstring GetStringW( int idx, const std::wstring& def ) const;
    float GetFloat1( int idx, float def ) const;     // find item in header and return value if exist otherwise return default value
    float GetFloat2( int idx, float def ) const;     // find item in header and return value if exist otherwise return default value
    bool GetFloat( int idx, float& f1, float& f2 ) const;     // find item in header and return value if exist otherwise return default value

    ZFSHeaderItem::type_t Type( int index ) const;    ///< return type of item

    bool SetDouble( int idx, double d );        // set value, add if does not exit otherwise change it
    bool SetLong( int idx, int32_t l);            // set value, add if does not exit otherwise change it
    bool SetStringC( int idx, const char* s );   // set value, add if does not exit otherwise change it
    bool SetStringW( int idx, const std::wstring& s );   // set value, add if does not exit otherwise change it
    bool SetFloat( int idx, float f1, float f2 );// set value, add if does not exit otherwise change it

    bool isModified() const { return modified; }      ///< return true if anything has changed
    void resetModified() { modified = false; }  ///< reset the modified flag. used for testing. Write reset the flag as well. Should be not called by default

    /// Pointer to first item in header !! not thread save
    ZFSHeaderItem* First() { return ptr = reinterpret_cast<ZFSHeaderItem*>(pParam.data()); }   
    /// get next item in header. Return null if end reached
    ZFSHeaderItem* Next();                                          
    // Thread save version
    ZFSHeaderItem* getFirst() { return reinterpret_cast<ZFSHeaderItem*>(pParam.data()); }
    const ZFSHeaderItem* getFirst() const { return reinterpret_cast<const ZFSHeaderItem*>(pParam.data()); }
    ZFSHeaderItem* getNext(ZFSHeaderItem* current);
    const ZFSHeaderItem* getNext(const ZFSHeaderItem* current) const;

    char* Buf()                 { return pParam.data(); }          // return buffer of items
    const char* Buf() const     { return pParam.data(); }
    char* BufHead()             { return reinterpret_cast<char*>(&head); }    // return pointer to header of start
    const char* BufHead() const { return reinterpret_cast<const char*>(&head); }


    // support lowercase
    double      getDouble( int idx, double def ) const          { return GetDouble(idx,def); }
    int32_t     getLong  ( int idx, int32_t def) const          { return GetLong(idx,def); }
    const char* getStringC( int idx, std::string& def ) const   { return GetStringC(idx,def); }
    std::string getStringC( int idx, const std::string& def ) const  { return GetStringC(idx,def); }
    const wchar_t* getStringW( int idx, std::wstring& def ) const { return GetStringW(idx,def); }

    bool setDouble( int idx, double d )          { return SetDouble(idx,d); }
    bool setLong  ( int idx, int32_t l )         { return SetLong(idx,l); }
    bool setStringC( int idx, const char* s )    { return SetStringC(idx,s); }
    bool setStringC(int idx, const std::string& s) { return SetStringC(idx, s.c_str()); }
    bool setStringW( int idx, const wchar_t* s ) { return SetStringW(idx,s); }
    bool setStringW(int idx, const std::wstring& s) { return SetStringW(idx, s.c_str()); }

    // low level functions
    int getTyp( int idx ) const;

    /*
    \brief zfsheader callback funktion
    \param obj the modified header
    \param var the item which is modified
    \param data
    \param idx the index of the value
    */
    typedef void( *cbHeaderHandler_f )( ZFSHeader* obj, const ZFSHeaderItem* var, const char* data, int idx );
    static void setHeaderHandler( cbHeaderHandler_f );

    void lockCallback( bool lock ){ mUseCallback = lock; }

    /// internal
    static void setMode(const std::string& key);
    static int getMode();

private:
    ZFSHeader(const ZFSHeader&) = delete;
    ZFSHeader& operator=(const ZFSHeader&) = delete;
protected:
    const ZFSHeaderItem* Find__(int idx) const;     ///< find item in header and return a pointer to it. Null if item not found (without mutex)

protected:
    static cbHeaderHandler_f cbHeaderHandler;
};
