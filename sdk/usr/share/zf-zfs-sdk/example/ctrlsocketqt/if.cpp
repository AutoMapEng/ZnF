
#include <math.h>
#include <stdio.h>

#include "if.h"
#include <zfs/zfs.h>

namespace zf
{
namespace ctrl
{

#ifdef USE_INTERFACE
 void MyLRC::OnConnect()
{
}
#endif

//-----------------------------------------------------------------------------------------------

/*
            receive LineData over UDP/IP
*/
#ifdef USE_LINESTREAM


class MyZFLinedataStream : public ZFLinedataStream
{
    int toggle; // toggle color of cut for every line
    bool mShowFilter;

public:
    MyZFLinedataStream() : ZFLinedataStream() { toggle = 0; mShowFilter = true; }

    void showFilter(bool show) { mShowFilter = show; }

    virtual void OnFilename(const int part, const char*& fn)
    {
        const int FileNameBufferSize = 1024;
        static char s[FileNameBufferSize];
#ifdef _WINDOWS
#if _MSC_VER < 1900
        sprintf(s, "c:\\scans\\profiler_%d.zfs", part);
#else
        snprintf(s, FileNameBufferSize, "c:\\scans\\profiler_%d.zfs", part);
#endif
#else
        snprintf(s, FileNameBufferSize, "/tmp/profiler_%d.zfs", part);
#endif
        fn = s;
    }

    // !!!! must be thread save !!!
    virtual void OnData(const ::lrcsocket::ZFStreamDeviceInfo&, const ::lrcsocket::LineRawInfo&, const char* /*head*/, const char* line, int pixel)  ///< receive full raw line. converts raw data to xyz and calls OnXYZ()
    {
    }
};
#endif // USE_LINESTREAM

//-----------------------------------------------------------------------------------------------

/*
            receive Statusinfo over UDP/IP
        */
#ifdef USE_INFOSTREAM
class MyZFInfoStream : public ZFInfoStream
{
public:

    // !!!! must be thread save !!!
    virtual void OnInfoStart()
    {
    }

    // !!!! must be thread save !!!
    virtual void OnInfo(int nr, int idx, const char* info, const char* val)
    {
    }

    // !!!! must be thread save !!!
    virtual void OnProgress(int percent)
    {
    }
};
#endif // USE_INFOSTREAM


//=========================================================================================



} // end namespace
} // end namespace
