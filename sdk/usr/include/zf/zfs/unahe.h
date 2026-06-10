#pragma once

#include "zfsapi.h"


typedef unsigned char unahe_t;

class ZFS_API CUnahe
{
    int deep;       // max 65536
    unahe_t* lut;
    
public:
    CUnahe();
	CUnahe( const CUnahe& );
    virtual ~CUnahe();
    
    bool Create( char* im, int sz, int deep=65536 );
    
    unahe_t operator()(int idx) const;

	unahe_t Get(int idx) const { return (*this)(idx); }
};



inline unahe_t CUnahe::operator()(int idx) const
{
    if( lut && idx < deep )
        return lut[idx];
    return 0;
}
