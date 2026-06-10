#ifndef __CMDPARSE_H__
#define __CMDPARSE_H__


class CParamParse
{
protected:
    CString cmd;

public:
    CParamParse( CString& str ) { cmd=str; }
    CParamParse( const char* str ) { cmd=str; }

    void Run();
    int getInt( const char* key, int def );
    bool get( const char* key );

protected:
    virtual void Parse( CString& cmd, CString arg ) = 0;
};



inline int CParamParse::getInt( const char* key, int def )
{
    char* ptr = cmd.GetBuffer( cmd.GetLength()+1 );
    char* p = strstr(ptr,key);
    if( p )
        def = atoi( &p[ strlen(key) + 1 ] );
    cmd.ReleaseBuffer();
    return def;
}

inline bool CParamParse::get( const char* key )
{
    return cmd.Find( key ) >= 0 ? true : false;
}

#endif //__CMDPARSE_H__
