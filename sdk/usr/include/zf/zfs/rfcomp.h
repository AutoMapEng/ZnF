#pragma once

#include "zfsapi.h"
#include "zfbase.h"

class ZFSpline;
class ZFSHeader;
class ZFLineAccess;
class ZFScan;
class ZFSLinebyline;


/**
	Intensity compensation depending on range and temperature

	! This is not a calibrated value !
	Only if scanner has the compensatins values and firmware since 7.3.x
*/
class ZFS_API ZFRfComp : public ZFBase
{
    DEFINE_ZFBASE(ZFBase)

protected:
    ZFSpline* pRfComp{};

public:
	ZFRfComp();
	ZFRfComp( ZFScan* );
	ZFRfComp( ZFSLinebyline* );
	ZFRfComp( ZFSHeader* );		// you have to create/delete object by your own
	virtual ~ZFRfComp();

    bool Available() const { return pRfComp ? true : false; } // true is scan has all the required values for compensating

    bool Setup(const ZFSHeader*);		// you have to create/delete object by your own

    /// clear all to allow new setup
    void clear();

    /// call RfComp of spline class
    double RfComp(int rf, double refrf, double rg_meter) const;

protected:
	// use one of following. setup will take some time
	bool Setup( ZFScan* );
	bool Setup( ZFSLinebyline* );

};

