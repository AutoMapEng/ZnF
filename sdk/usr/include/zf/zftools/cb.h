#pragma once

#include "zftools_api.h"

#include <vector>

#pragma warning( push )
#pragma warning( disable : 4251 ) //Fehler beziht sich auf std::vector kann ignoriert werden

class ZFTOOLS_API ZFCB
{
public:
    typedef int (*cb_t)(int,int,const char* ,const char*); // definition der callback funktion return !0 to break calllist

    enum { RES_NEXT, RES_BREAK };

    void InstallCallback( cb_t cb );
    void RemoveCallbacks();
    int CbBreak( int act=0, int i=0, const char* p1=NULL, const char*p2=NULL); // alle callbacks der liste ausfuehren
    void CbAll( int act=0, int i=0, const char* p1=NULL, const char*p2=NULL); // alle callbacks der liste ausfuehren

protected:
    typedef std::vector<cb_t> cblist_t; // typdef der callbackliste
    cblist_t cblist;	// callbackliste

public:
    ZFCB();
    virtual ~ZFCB();
};

#pragma warning( pop )	//Alter status wieder hergestellt
