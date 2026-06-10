#ifndef __FHTOOLS_H__
#define __FHTOOLS_H__

///////////// Dfiniton ///////////////////

#ifdef UNICODE
#define FHTOOLS_STRING std::wstring
#define FH_SPRINTF swprintf
#define FH_STRLEN wcslen
#define FH_STRSTR wcsstr
#define FH_STRCAT wcscat
#define FH_STRRCHR wcsrchr
#define FH_LPCSTR LPCWSTR
#define FH_VSPRINTF wvsprintf
#define FH_STRNCMP wcsncmp
#define FH_STRDUP _wcsdup
#else
#define FHTOOLS_STRING std::string
#define FH_SPRINTF sprintf
#define FH_STRLEN strlen
#define FH_STRSTR strstr
#define FH_STRCAT strcat
#define FH_STRRCHR strrchr
#define FH_LPCSTR LPCSTR
#define FH_VSPRINTF vsprintf
#define FH_STRNCMP strncmp
#define FH_STRDUP _strdup
#endif

////////// Application ///////////////////
void SetPath4Ini( CWinApp* app, const TCHAR* ininame=NULL );

double  GetPrivateProfileDbl( FH_LPCSTR a, FH_LPCSTR b, double v, FH_LPCSTR ini );
double  GetPrivateProfileDbl( FH_LPCSTR a, int b, double v, FH_LPCSTR ini );
int     GetPrivateProfileInt( FH_LPCSTR a, int b, int v, FH_LPCSTR ini );
void    WritePrivateProfileInt( FH_LPCSTR a, FH_LPCSTR b, int v, FH_LPCSTR ini );
void    WritePrivateProfileDbl( FH_LPCSTR a, FH_LPCSTR b, double v, FH_LPCSTR ini );

const TCHAR* GetExePath();

///////////// Array /////////////
#ifndef deletex
 #define deletex(p) if(p) delete p; p=NULL;
 #define deletexa(p) if(p) delete [] p; p=NULL;
#endif

/////////// Windowplacement ////////////////
extern TCHAR BASED_CODE szSection[];

BOOL ReadWindowPlacement ( CWinApp* pApp, LPWINDOWPLACEMENT pwp, const TCHAR* szEntry, const TCHAR* szSec=szSection );
void WriteWindowPlacement( CWinApp* pApp, LPWINDOWPLACEMENT pwp, const TCHAR* szEntry, const TCHAR* szSec=szSection );


#define TOPMOSTWND     SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );


#define RED     RGB(128,0,0)
#define GREEN   RGB(0,128,0)
#define BLUE    RGB(0,0,128)
#define LRED    RGB(255,0,0)
#define LGREEN  RGB(0,255,0)
#define LBLUE   RGB(0,0,255)
#define GRAY    RGB(128,128,128)
#define LGRAY   RGB(192,192,192)
#define BLACK   RGB(0,0,0)
#define WHITE   RGB(255,255,255)
#define CYAN    RGB(128,0,128)
#define LCYAN   RGB(255,0,255)
#define MAGENTA RGB(0,128,128)
#define LMAGENTA RGB(0,255,255)
#define YELLOW  RGB(255,255,0)
#define BROWN   RGB(128,128,0)


/////////////// Filename handling ////////////////////


void AddExt( TCHAR* fn, const TCHAR* ext );
void AddExt( CString& fn, const TCHAR* ext );
void IncrementFileName( TCHAR* file );


bool Check4Exec( const TCHAR* szApp );
bool BringToTop( const TCHAR* szApp ); // return true if app found
bool TopMost( const TCHAR* szApp );
void Exec( const char* szApp, unsigned int style=SW_NORMAL );


void AfxMessageBoxLastError( const TCHAR* msg=NULL );
int MyMessageBox( const TCHAR* format, ... );
const TCHAR* GetLastErrorString();

void Log( FH_LPCSTR string, ... );
void Log( FH_LPCSTR filename, FH_LPCSTR string, ... );
void LogDebug( FH_LPCSTR string, ... );
enum { ERR, WARN, INFO };
void Log( int typ, FH_LPCSTR string, ... );
void Log( int typ, int err );

long PlugIn( const TCHAR* dll, const char* szf, const TCHAR* param1, const TCHAR* param2=NULL, const TCHAR* param3=NULL );


/////////////// MISC ////////////////////

const TCHAR* Bin( int value, int n );



#endif //__FHTOOLS_H__