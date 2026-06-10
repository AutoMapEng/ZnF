#ifndef ZFSFILTER_INTERFACE_H
#define ZFSFILTER_INTERFACE_H

#include "filterapi.h"

#include <string>


namespace zfsfilter
{

class FILTERDLL_API ZFSFilterInterface
{
public:
    virtual int Filter() = 0;
    
    virtual void progressStart( int max, const std::wstring& info ) {}
    virtual bool progress( int i ) { return true; }
    virtual void progressEnd() {}
};

}; // end namespace

#endif // ZFSFILTER_INTERFACE_H
