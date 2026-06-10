#ifndef __COMMONCMD_H__
#define __COMMONCMD_H__


//
// Interface between MC Motorola2107 and PC
//

#ifndef CMD_MC_SPACE
#define CMD_MC_SPACE			64
#define CMD_MCLOCALADRESSIN		0x8001ff00
#define CMD_MCLOCALADRESSRET	0x8001ff40
#define CMD_MCLOCALADRESSCMD	0x8001ff80
#endif

#define ADR_WRBLOCK_BUFFER      0x8001e000
#define WRBLOCK_SIZE            0x1000

#define ADR_FLASH_FPGA          0x81010000  // 0x70000 = 458 kByte
#define ADR_FLASH_LUT0          0x81080000  // 0x40000 = 256 kByte
#define ADR_FLASH_LUT1          0x810c0000  // 
#define ADR_FLASH_LUT2          0x81100000  // 
#define ADR_FLASH_LUT3          0x81140000  // bis 0x81180000
#define ADR_FLASH_BDE           0x811c0000

#define ADR_FLASH_BDE_SIZE      0x40000

#define FLASH_HEADER            16
#define SIZE_FLASH_RAWLUT       0x4000          // maximum size   16384 Bytes   1250 items (20000>>4) * short * 4 = 10000
#define ADR_FLASH_RAWLUT1       ADR_FLASH_LUT2  // 
#define ADR_FLASH_RAWLUT2       ADR_FLASH_LUT3  // 


//-------------------------

#define CMD_VERSION		    1
#define CMD_RESET           2
#define CMD_ERROR_CLR       4
#define CMD_ERROR           5
#define CMD_HARDWARE		6		// check for hardware
#define CMD_PARAMS		    8		// zusaetzliche parameter
#define CMD_WRADR		    12

#define CMD_BUF_PARSE       20
#define CMD_BUF_WRFLASH     21
#define CMD_ERASEFLASH      22

#define CMD_CRC             50

#define CMD_BEEP            100
#define CMD_O               101
#define CMD_I               102
#define CMD_FPGARELOAD      110

#define CMD_AD              120
#define CMD_BAT             121

#define CMD_DDCON           200
#define CMD_LASER           201
#define CMD_DDCRESET        202
#define CMD_LUEFTER         205

#define CMD_PARAM_SET       400
#define CMD_PARAM_GET       401
#define CMD_PARAM_GETBYNR   402
#define CMD_PARAM_CRC       403
#define CMD_PARAM_SETENABLED  405
#define CMD_PARAM_USED      407     // since V5.3.0    -> used, max


#define CMD_TEMP		    600
#define CMD_FPGARD		    601
#define CMD_FPGAWR		    602
#define CMD_TEMPCOMP	    603
#define CMD_APD_RD		    604
#define CMD_POS			    605

#define CMD_SCANSTART       1000
#define CMD_SCANSTOP        1001
#define CMD_FILTER_SET	    1005
#define CMD_FILTER_GET	    1008
#define CMD_DDCRANGE_SET    1006
#define CMD_DDCRANGE_GET    1009

#define CMD_DDCRD16         1100
#define CMD_DDCRD40         1101
#define CMD_DDCWR           1102
#define CMD_DDCWRRD         1103

#define CMD_BDE             1200    // check if bde available
#define	CMD_BDE_RD			1201
#define	CMD_BDE_WR			1202
#define CMD_BDE_CLEAR       1203    // p1=password "7964"
#define CMD_BDE_SCAN        1205
#define CMD_BDE_CLR         1206    // erase prepayed counter block 0..n
#define CMD_BDE_ADR         1207
#define CMD_BDE_SIZE        1208
#define CMD_BDE_ITEMS       1209


#define CMD_DC			    2000
#define CMD_SM			    3000

//-------------------------

#define CMD_DC_VER          1
#define CMD_DC_REF		    1000
#define CMD_DC_MOVE		    1003
#define CMD_DC_POWER		1025
#define CMD_DC_SPEED		1002
#define CMD_DC_SPEED100		1027
#define CMD_DC_STOP			1011
#define CMD_DC_HOME			1026

//-------------------------

#define CMD_SM_VER          1
#define CMD_SM_REF			2000
#define CMD_SM_GOTO			2001
#define CMD_SM_SPEED		2002
#define CMD_SM_STOP			2010
#define CMD_SM_INT			2012


//-------------------------


#define CMD_LIBELLE_CMD     4000
#define CMD_LIBELLE_RET     4001


//-------------------------

#endif //__COMMONCMD_H__
