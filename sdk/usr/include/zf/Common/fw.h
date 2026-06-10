#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef _WINDOWS
# ifdef FW_EXPORTS
#  define FW_API __declspec(dllexport)
# else
#  define FW_API __declspec(dllimport)
# endif
#else
# define FW_API
#endif


#ifndef RED
# define RED     RGB(128,0,0)
# define GREEN   RGB(0,128,0)
# define BLUE    RGB(0,0,128)
# define LRED    RGB(255,0,0)
# define LGREEN  RGB(0,255,0)
# define LBLUE   RGB(0,0,255)
# define GRAY    RGB(128,128,128)
# define LGRAY   RGB(192,192,192)
# define BLACK   RGB(0,0,0)
# define WHITE   RGB(255,255,255)
# define CYAN    RGB(128,0,128)
# define LCYAN   RGB(255,0,255)
# define MAGENTA RGB(0,128,128)
# define LMAGENTA RGB(0,255,255)
# define YELLOW  RGB(255,255,0)
# define BROWN   RGB(128,128,0)
#endif


class FW_API ZFProgress
{
public:
    ZFProgress( int from, int to, const char* desc, bool abort ) {}
    virtual ~ZFProgress() {}

    virtual bool Cancel()                   { return false; }
    virtual void Set( int pos )             {}
    virtual void Set( const char* text )    {}
    virtual void SetRange( int from, int to ) {}
};


enum MB { MBOK, MBYESNO }; // MessageBox-Param
enum FW { MBRET_NO=0, MBRET_CANCEL=0, MBRET_YES=1, MBRET_OK=1 }; // MessageBox-Return

// mig [3/14/2003] 
//mignotify
//for the framework we are granting access to CLara
//this helps in collecting data for broadcasts
class CLara;

class FW_API ZFFrameWorkAPI
{
    bool bDebug;

public:
    ZFFrameWorkAPI() { bDebug=false; }
    virtual ~ZFFrameWorkAPI() {}

    virtual void ClearScreen() {}
    virtual void Printf( const char*, ... ) {}
    virtual void Printf( DWORD col, const char*, ... ) {}
    virtual void PrintfDebug( const char*, ... ) {}
    virtual void PrintfError( const char*, ... ) {}
    virtual void PrintfWarning( const char*, ... ) {}
    enum { COL_ERR=RGB(255,0,0), COL_WARN=RGB(128,128,0) };

    enum { MBOK, MBYESNO }; // Param
    enum { MBRET_NO=0, MBRET_CANCEL=0, MBRET_YES=1, MBRET_OK=1 }; // Return
    virtual int MessageBox( const char*, int=MBOK ) { return MBRET_CANCEL; }

    enum { FATAL, ERR, WARN, INF };
    virtual void Log( int typ, const char*, ... ) {}

    virtual ZFProgress* NewProgress( int from, int to, const char* desc=NULL, bool abort=false ) { return NULL; }

    virtual void SetDebug( bool d ) { bDebug=d; }
    virtual bool IsDebug() { return bDebug; }

	// mig [3/14/2003] 
	//mignotify
	class Notification
	{
	public:
		enum Type {ScanStopped,DevChanged,Ready,Success,Failure};
	};
	virtual bool broadcastNotification(Notification::Type,CLara*) {return false;}
};




#endif //__FRAMEWORK_H__
