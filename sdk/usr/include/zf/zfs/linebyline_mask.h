#pragma once
//
// Line by line access for zfs scans
//
//   very fast access to lines
//
#include "zfsapi.h"

#include <string>
#include <list>
#include <vector>

#include "zfbase.h"
#include "zfvector.h"

//#############################################################################################################

class ZFSHeader;
class PartInf;
class ZFMask;
class ZFMaskPartial;
class ZFSLinebyline;
namespace zfs { namespace math { class rgb; } }


class ZFS_API ZFMaskParialLineByLine : public ZFMaskPartial
{
    PartInf* newParent{};

public:
    ZFMaskParialLineByLine(PartInf* p, ZFMask* glb) : ZFMaskPartial(0, glb) { newParent = p; }

    virtual void Clr(int x_, int y_, bool map = true) override;
    virtual void Set(int x_, int y_, bool map = true) override;

    virtual void SetNoSync(int x_, int y_) override;
    virtual void ClrNoSync(int x_, int y_) override;
};


//#############################################################################################################


/**
@brief holds all masks of a single part of the scan
*/
class LineByLineMasksOfPart : public ZFBase
{
    DEFINE_ZFBASE(ZFBase)
    
    friend class ZFSLinebyline;
    friend class ZFMaskParialLineByLine;

protected:
    ZFSHeader* h{};
    PartInf* parent{};        ///< the part the masks belong to
    std::wstring mFileName;	///< Name of the File;
    int pixel{};
    int lines{};
    
public:
    ZFMask globalmask;
    std::list <ZFMaskPartial*> mask_list;

    typedef std::vector<ZFBase*> objlist_t; 
    objlist_t objlist;  //!< Holds variable kind of objects assosiated with scan. Sestructor frees objects.

    ZFBase*  addObj( ZFBase* ); //!< Add new object.  
    bool ZFS_API removeObj( ZFBase* ); //!< Remove the object. 
    void ZFS_API  removeObjAll( const char* name=NULL ); //!< Remove all objects. Use name to remove all objects of one typ.    

    typedef void (*cbObj_t)( objlist_t&, bool );
    cbObj_t cbObj{};
    typedef void (*cbMask_t)( int action ); //!< Called on add or remove mask.
    cbMask_t cbMask{};

    ZFMask* AddMaskInt(int x, int y, int w, int h, const zfs::math::rgb& rgb, const std::wstring& name, bool set);
    int getPixel() const { return pixel; }

public:
    ZFS_API LineByLineMasksOfPart( PartInf* p, ZFSHeader* header, int pix, int lin );
    virtual ZFS_API ~LineByLineMasksOfPart();

    ZFMask* AddMask(const zfs::math::rgb& rgb, const std::wstring& name=L"", bool set = true);
    virtual int ZFS_API writeMasks2File(); //!< Attach all masks to file.
    ZFMaskPartial* FindMask( const std::wstring& name ) const;
    int ZFS_API readMasks( const std::wstring& fileName );
    bool ZFS_API RemoveMask( ZFMask*, bool sync=true );
    void ZFS_API SyncMask();
    bool ZFS_API WriteHeader();

    bool ZFS_API IsSet( int x, int y ) const;
};


//#############################################################################################################

