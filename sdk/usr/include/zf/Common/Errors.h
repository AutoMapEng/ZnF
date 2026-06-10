#ifndef __ERRORS_H__
#define __ERRORS_H__


#define MAX_ERRORS  100


void ErrorClr();
void Error( long err, const char* str=NULL );
long LastError();
long FirstError();
long NextError();
const char* ErrorString( long err );
void Warning( long warn, const char* str=NULL );



const char* ErrorStringFW( int nr );
const char* ErrorStringDC( int nr );
const char* ErrorStringSM( int nr );

#define ERR_ASFW                0x00010000
#define ERR_BAT_LOW             0x00010001
#define ERR_BAT_HIGH            0x00010002
#define ERR_START_SCAN          0x00010003
#define ERR_FPGA                0x00010004
#define ERR_AS_POWER            0x00010005
#define ERR_AS_RANGE            0x00010006
#define ERR_AS_FILTER           0x00010007
#define ERR_TEC                 0x00010008
#define ERR_IMON                0x00010009
#define ERR_TEMP_AS_HIGH        0x00010010
#define ERR_TEMP_AS_LOW         0x00010011
#define ERR_TEMP_AS_COMP        0x00010012
#define ERR_TEMP_ASFW_HIGH      0x00010013
#define ERR_TEMP_ASFW_LOW       0x00010014
#define ERR_SECURITY_KEY        0x00010015
#define ERR_DEBUG_KEY           0x00010016
#define ERR_SETUP_LASER         0x00010017
#define ERR_1394_TIMEOUT        0x00010018
#define ERR_CHECK_ONLINE        0x00010019
#define ERR_BUFFER_OVERRUN      0x0001001a 
#define ERR_SETUP_LASER_RANGE   0x0001001c


#define ERR_DC                  0x00020000
#define ERR_DC_REF              0x00020001
#define ERR_DC_START            0x00020002
#define ERR_DC_SPEED            0x00020003

#define ERR_SM                  0x00030000
#define ERR_SM_REF              0x00030001
#define ERR_SM_START            0x00030002

#define ERR_HD_OUT_OF_SPACE     0x00040000
#define ERR_CPU_TO_SLOW         0x00040001
#define ERR_SEND_CMD            0x00040002
#define ERR_TRIGGER             0x00040003
#define ERR_TRIGGER_TO          0x00040004
#define ERR_REFERENZ            0x00040005
#define ERR_SCAN_ENC            0x00040006



#endif //__ERRORS_H__
