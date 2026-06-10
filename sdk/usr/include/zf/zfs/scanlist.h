#pragma once

#include "zfsapi.h"
#include "zfserr.h"
#include "zfsdefs.h"

#include <zfcore/filename.h>

#include <list>
#include <vector>


//
// there is one static ZFScanList which holds all opened scans
//

class ZFScanBasic;
class ZFScan;

typedef std::list <ZFScanBasic*> scanbasic_lst;

/**
\brief holds a list of all open scans. Uses callbacks as well
*/
class ZFS_API ZFScanList
{
    friend class ZFScanBasic;

public:
    ZFScanList();
    virtual ~ZFScanList();

    enum { ADDED=1, REMOVED=-1, CHANGED=0 }; // callback typ

    int Count();	// number of opend scans
    scanbasic_lst List();

    virtual ZFScan* Open( const wchar_t* filename, const char* param=NULL, int flags=OPEN_DEFAULTFLAGS_ZFSCAN, zfs::zfserr_t* err=NULL );   /// UTF16 open scan, see zfsdefs.h for openflags
    virtual ZFScan* Open( const std::wstring& filename, const char* param=NULL, int flags=OPEN_DEFAULTFLAGS_ZFSCAN, zfs::zfserr_t* err=NULL );   /// UTF16 open scan
    virtual ZFScan* Open( const std::string& filename, const char* param=NULL, int flags=OPEN_DEFAULTFLAGS_ZFSCAN, zfs::zfserr_t* err=NULL );   /// UTF8
    ZFScan* Open( const zf::FileName& filename, const char* param=NULL, int flags=OPEN_DEFAULTFLAGS_ZFSCAN, zfs::zfserr_t* err=NULL );   /// with filename class
    virtual bool Close( ZFScanBasic*, bool useCallback = true );
    virtual void CloseAll();

    virtual ZFScan* Current();              /// current active view
    virtual bool Current( ZFScan* cur, bool useCallback = true );    /// set current

    ZFScanBasic* FindExact( const char* );
    ZFScanBasic* FindExact( const std::wstring& fn );
    ZFScanBasic* Find( const char* ); //TODO loeschen
    ZFScanBasic* Find( const std::wstring& fn );
    const ZFScanBasic* Find(const std::wstring& fn) const;
    bool Find( ZFScanBasic* );

    bool isOpen(const std::wstring& fn);

    typedef void (*cb_t)(int typ, ZFScanBasic* scan, void* contex);

    void InstallCallback( cb_t c, void* con=NULL );
    void DestroyCallbacks();


protected:
    scanbasic_lst zflist;					// list of all documents (scans)
    scanbasic_lst::iterator zflist_it;		// current/last active document

    // Callbacks
    struct ZFcb
    {
        cb_t cb;
        void* contex;
        ZFcb(cb_t c, void* p) { cb = c; contex = p; }
    };
    std::vector<ZFcb> cb;	// open/close callback list

    /**
    * @brief Adds the given scan to the scan list.
    *
    * @param scan the scan to add
    * @param useCallback flag, if callbacks are send
    */
    void Add(ZFScanBasic* scan, bool useCallback = true);

    /**
    * @brief Removes the given scan from the scan list.
    *
    * @param scan the scan to be removed
    * @param useCallback flag, if callbacks are send
    */
    void Remove(ZFScanBasic* scan, bool useCallback = true);
};


//------------------------------------------------------------------------------------


extern ZFS_API ZFScanList scanlist; /// holds all open scans


/**
*	Removes the scanfilte and all additional tmp files
*/
ZFS_API void RemoveScanFromDisk( const std::wstring fn );


