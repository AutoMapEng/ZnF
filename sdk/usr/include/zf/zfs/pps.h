#ifndef __ZFS_PPS_H__
#define __ZFS_PPS_H__

#include "zfsapi.h"

#include <string>


class ZFSLinebyline;


namespace zfs
{

/**
\brief  Detect PPS synchronisation and calculates start values (obsolete). Please use new sync API
*/
class ZFS_API PPSSync
{
public:
    bool bInit;
    int  iFirstLineCntChange;			/// first line where counter changes
	int  iPixelCounterChange;			/// the pixel in the line where counter changed
    int  iCntFirstCntChange;			/// value of counter after first change
    int  iTimeFirstCntChange;			/// time [ms] from start of file until first counter change
    int  iTimeUtc2FirstCntChangeInData; /// time [ms] from receiving UTC message from GPS until first counter change in scandata
    int  iTimeUtc2FirstCntChange;		/// time [ms] from receiving UTC message from GPS until first counter change to 1
    int  iUtcTimeAtCounterZero23002;	/// hhmmss from zfs-header index 23002
	int  iGpsWeekSecondAtCounterZero23008;	/// seconds from zfs-header index 23008
	int  iGpsWeekAtCounterZero23009;    /// week from zfs-header index 23009
    //int  iCorrectCnt;					/// increments you should add to counter for example if -pps is used with -cnt1reset
    
    PPSSync();

	void ResetValues();					/// reset variables

	/**
	\param iTimePPS2MSG	[ms] the time from PPS Puls until ZDA message is received. Depends on baudrate and number of messages gps sends
	*/
    bool Init( ZFSLinebyline*, int iTimePPS2MSG=0 );

	std::string Dump( const char* cr="\n" );
};

//===========================================================

class ZFS_API UTCTime
{
public:
    int iTyp;
    enum { T_UNDEF };
    
    int iYear;
    int iMonth;
    int iDay;
    int iHour;
    int iMin;
    int iSec;
    int iMs;
    
    UTCTime() { iTyp=T_UNDEF; }
};




ZFS_API UTCTime AbsTime( ZFSLinebyline*, int ln, int px=0 );  // return [ms]

} // end namespace zfs

#endif //__ZFS_PPS_H__
