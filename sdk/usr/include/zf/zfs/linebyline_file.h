#pragma once
//
// Line by line access for zfs scans
//
//   very fast access to lines
//

#include <string>
#include <memory>
#include <vector>

#include "map.h"
#include "zfsapi.h"
#include "zfbase.h"
#include "zfserr.h"
#include "layer_impl.h"
#include <zfcore/filename.h>


//#############################################################################################################


class ZFSLinebyline;
class LineByLineMasksOfPart;
namespace zf
{
    class ZFFile;
}


//--------------------------------------------------------------------------------------------------------------------

class ZFReader
{
public:
    ZFSLinebyline* parent{};
    int32_t absLine{-1};

protected:
    ZFReader();

public:
    ZFReader( ZFSLinebyline* pa );
    virtual ~ZFReader() {}

    virtual int32_t Id() const { return 0; }

    virtual bool CanRandomAccess() const { return false; }
    virtual zfs::zfserr_t Open( int32_t flags, ZFSLinebyline* zfs ) = 0;
    virtual void Close() = 0;

    virtual int32_t SkipTo( int32_t line, bool reload = false ) = 0;
    virtual int32_t LoadLines( int32_t lines, bool reload = false ) = 0;
    virtual int32_t LoadLineHeader( int32_t ln, char* buf, int32_t bufsize  ) = 0;
    
    /// get the file position of the asked line
    /// \return 0 if not possible
    virtual int64_t determineFilePos(int32_t ln) = 0;

    virtual ZFReader* Copy()= 0;

    int32_t currentAbsLine() const { return absLine; }
};

#pragma pack(1)
struct LineInf //PG 7.2.0.4
{
    uint64_t linePos;
    uint32_t lineInFile;

    LineInf(uint64_t lp, uint32_t lif )
        : linePos(lp), lineInFile(lif) {}

    LineInf()
        : linePos(0u), lineInFile(0u) {}
};
#pragma pack()

static_assert(sizeof(LineInf) == 12, "struct LineInf: Size missmatch");

//--------------------------------------------------------------------------------------------------------------------

class ZFReadFile;

class ZFS_API VecList
{
    ZFReadFile* mParent;
    std::vector<int> mPartList;
    LineInf* mList;
    int32_t mSize;

    LineInf mNullVel;

public:
    //VecList() : mParent(NULL), mList(NULL), mSize(0) {}
    VecList( ZFReadFile* parent ) : mParent(parent), mList(NULL), mSize(0) {}
    VecList( const VecList& o ){*this = o;}
    ~VecList() { clear(); }

    void setParent( ZFReadFile* parent ) { mParent = parent; }
    void resize( int32_t s );
    LineInf& operator[]( int32_t at );
    zf::ZFFile* getFP( int32_t at ) const;
    void setPart( int32_t at, int32_t part );
    int getPart( int32_t at ) const { return at < mSize? mPartList[at]:0;}
    void clear();

    VecList& operator=( const VecList& o );

    int32_t size() const {return mSize;}
};


//--------------------------------------------------------------------------------------------------------------------

class ZFS_API PartInf //7.0.0.10 PG warning C4099
{
public:
    int32_t mflags{};
    zf::FileName fileName;
    zf::ZFFile* fp{};
    ZFSHeader* header{};
    int32_t lines{};
    int32_t successRd{};
    int32_t startLine{};
    ZFScanMap* pixLineMap{}; // PG read rgb jpg und Mask
#ifdef LBL_MASK 	//7.0.0.10 PG
    LineByLineMasksOfPart* masksOfPart{};	//!< all Masks of this file.
    ZFMask* currentMask{};    //!< Current aktive Mask
    std::vector<std::shared_ptr<zfs::ZFLayer>> layerMasks{};


    void ReadMask();
    bool WriteMask();
    bool AddMask(zfs::math::rgb rgb, const std::string& name, bool set );
    bool AddMask(zfs::math::rgb rgb, const std::wstring& name, bool set );
    bool FindMask( const std::string& name );
    bool FindMask( const std::wstring& name );
    ZFMask* findMask(const std::wstring& name) const;
    bool IsSetMask( int32_t x, int32_t curLine ) const;
    void SetMask( int32_t p, int32_t l )
    {
        if( currentMask )
            currentMask->Set(p,l);
    }
    void ClrMask( int32_t p, int32_t l )
    {
        if( currentMask )
            currentMask->Clr(p,l);
    }

#endif
    
    PartInf( int32_t flag );
    PartInf( const PartInf& o );

    ~PartInf();

    PartInf& operator=(const PartInf& o);

    // PG 7.2.0.4
    bool readFile(int32_t part, VecList& vp, int32_t start, int32_t flags = 0);

    // remove all layers added to the part
    void removeLayers();
};

//--------------------------------------------------------------------------------------------------------------------

class ZFReadFile : public ZFReader
{
protected:
    //std::vector<LineInf> lineInf;//PG 7.2.0.4
    VecList lineInf;

public:
    bool first{};

    // file
    bool ok{};					//!< True if read file ok.

    std::string currentFilename;

    int32_t linesCurrent{};
    int32_t linesAllParts{};
    int32_t parts{};
    int32_t currentPart{};

    std::vector<PartInf> partinf{};

public:
    ZFReadFile( ZFSLinebyline* pa=NULL );
    ZFReadFile( const ZFReadFile& o );
    virtual ~ZFReadFile() {}

    bool isOk() const { return ok; };	//!< Returns the status of the read file.

    virtual zfs::zfserr_t Open( int32_t flags, ZFSLinebyline* zfs  ) override;
    virtual void Close() override;

    const char* CurrentFileName() { return partinf[CurrentPart()].fileName.c_str(); }
    const wchar_t* CurrentFileNameW() { return partinf[CurrentPart()].fileName.w_str(); }
    int32_t CurrentPart();

    /**
     * get line number in partand par having a abs line number.
     * 
     * \param line_abs = absolute line number
     * \return first = part -1 if no parts
     *         second = relative line number
     */
    std::pair<int, int> getPartAndRellinenumber(int line_abs) const;


    zf::ZFFile* getFile( int32_t i );

    int32_t LinesCurrentFile() const { return linesCurrent; }
    int32_t GetCurrentStartLine() const { return partinf[currentPart].startLine; }

    int32_t Parts() const { return parts; }
    const wchar_t* Part(int32_t i) const { if(i<parts) return partinf[i].fileName.w_str(); else return NULL; }

    virtual int32_t SkipTo( int32_t line, bool reload = false ) override; // only if CanRandomAccess is true otherwise no action
    virtual int32_t LoadLineHeader( int32_t ln, char* buf, int32_t bufsize  ) override;
    virtual int64_t determineFilePos(int32_t ln) override;

    bool WriteCurrentLine();
    bool WriteCurrentLineHeader();
    bool WritePreLineHeader();

    bool WriteAllHeader( int32_t idx, int32_t value );
    bool WriteAllHeader( int32_t idx, double value );

    ZFReadFile& operator=( const ZFReadFile& o );

    const std::vector<PartInf>* getPartList() const { return &partinf; }
};



//--------------------------------------------------------------------------------------------------------------------

class ZFReadFileSingleLine : public ZFReadFile
{
public:
    ZFReadFileSingleLine( ZFSLinebyline* pa );
    virtual ~ZFReadFileSingleLine() {}

    virtual int32_t Id() const override { return 1; }

    virtual bool CanRandomAccess() const override { return true; }

    virtual int32_t LoadLines( int32_t lines=1, bool reload = false ) override;
    virtual ZFReader* Copy() override;
};

//--------------------------------------------------------------------------------------------------------------------

class ZFReadFileRollBuffer : public ZFReadFile
{
public:
    ZFReadFileRollBuffer( ZFSLinebyline* pa );
    virtual ~ZFReadFileRollBuffer() {}

    virtual int32_t Id() const override { return 2; }

    virtual bool CanRandomAccess() const override { return true; }

    virtual int32_t LoadLines( int32_t lines, bool reload = false ) override;
    virtual ZFReader* Copy() override;
};

//--------------------------------------------------------------------------------------------------------------------

ZFS_API std::wstring NextName( ZFSHeader* h );

//#############################################################################################################
