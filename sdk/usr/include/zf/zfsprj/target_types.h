#pragma once


//Darf nicht groesser als 9 werden da ab 10 der vom user zu belegende Bereich beginnt.
enum eTargetTyp
{
    TARGET_TYP_AUTO = 0,
    TARGET_TYP_CHESS = 2,		//!< chess pattern target
    TARGET_TYP_SPHERE = 5,	//!< sphere
    TARGET_TYP_XYZ = 6,		//!< target set manuell
    TARGET_TYP_CROSS = 7,
    TARGET_TYP_ELLIPSE = 8,
    TARGET_TYP_PROFILER = 9,	//!< global target from a profiler scan
    TARGET_TYP_APRIL = 1,      //!< april target
    TARGET_TYP_FIT = -2,		//!< position fitted by corner or edge

    TARGET_TYP_UNDEF = 0xffffffff
};

#define TARGET_TEMP_PATH		".target"	///< where targetimages and found targets are stored
#define TARGET_UNNAMED_ID_STR	"?"			///< identifier unnamed target
#define TARGET_UNNAMED_ID_WSTR	L"?"
#define TARGET_UNNAMED_ID_CHAR	'?'
#define TARGET_UNNAMED_ID_WCHAR	L'?'
