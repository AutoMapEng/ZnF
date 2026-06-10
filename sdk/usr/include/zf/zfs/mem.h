#ifndef __ZFSDLL_MEM_H__
#define __ZFSDLL_MEM_H__

#include "zfsapi.h"

#include "config.h"
#ifdef ZFS_MEM

class ZFScan;



//
// holds complete range or intensity memory
//
class ZFS_API ZFMem
{
protected:
    int sizex;
    int sizey;
    int deep;
    char* data;

    double avr;
    double std;

public:
    ZFMem() { data=0; deep=0; sizex=sizey=0; avr=std=0.; }
    ZFMem( int x, int y, int sz ) { Alloc(x,y,sz); }
    ~ZFMem() { Free(); }

    char* Alloc( int x, int y, int sz );
    char* Alloc( ZFScan*, int idx );
    void Free();

    int SizeX() { return sizex; }
    int SizeY() { return sizey; }
    int Deep()  { return deep; }

    char* Data() { return data; }
    char* Data( int l ) { return data + sizex*deep*l; }
    char* Data( int x, int y ) { return data + sizex*deep*y + deep*x; }

    double Avr() { return avr; }
    double Std() { return std; }
};

#endif // ZFS_MEM


#endif //__ZFSDLL_MEM_H__
