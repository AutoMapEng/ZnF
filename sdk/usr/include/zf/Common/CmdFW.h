




enum { CMDFW_UNKNOWN, CMDFW_ASC, CMDFW_DATA, CMDFW_VALUE };
enum { ERR_NO, ERR_SENDFW, ERR_RECFW, ERR_CMD_UNKNOWN };
enum { FW_RD, FW_WR };

struct SCmdFwHeader
{
    DWORD   len;
    BYTE    kenn[4];
    DWORD   typ;
    DWORD   ver;

    void Init( WORD t, WORD v  ) { len=sizeof(SCmdFwHeader); kenn[0]='L'; kenn[1]='A'; kenn[2]='R'; kenn[3]='A'; typ=t; ver=v; }
    bool Check() { return kenn[0]=='L' && kenn[1]=='A' && kenn[2]=='R' && kenn[3]=='A' ? true : false; }

    void SetLen( DWORD l ) { len = l; }
};




//------------ CMDFW_ASC ------------------------------

#define MAX_ASC  80

struct SCmdFwAsc
{
    char  cmd [MAX_ASC];
    DWORD len;

    SCmdFwAsc() 
    {
        cmd[0] = 0;
        len = 0;
    }
    SCmdFwAsc( const char* s ) 
    { 
        strncpy( cmd, s, MAX_ASC );
        cmd[MAX_ASC-1] = 0;
        len = strlen(cmd );
    }
};

//------------ CMDFW_Data ------------------------------



struct SCmdFwData // typ:1 ver:1
{
    DWORD   typ;
	DWORD   cmdfw; //Befehl an FireWire
	DWORD   p1;
    DWORD   p2;
    DWORD   p3;
    DWORD   p4;
	DWORD   p5;
	DWORD   p6;
	DWORD	time; //timeout for Firewire

	void Init(){ typ=0; cmdfw=0; p1=0; p2=0; p3=0; p4=0; p5=0; p6=0; }

//    virtual const char* Name() = 0;
};


//------------ CMDFW_VALUE ------------------------------


struct SCmdFwValue // typ:1 ver:1 //Single value
{
    DWORD   typ;
	DWORD   val;  //sh. CLaraFireWire::Rd
	DWORD   adr;
	DWORD	adrh; 
	DWORD	n;

	void Init(){ typ=0; val=0; adr=0; adrh=0; n=4; }

//    virtual const char* Name() = 0;
};



//------------ CMDFW_Result ------------------------------

struct SCmdFwResult // typ:1 ver:1
{
    DWORD   error;
	DWORD   cmdfw;  //Befehl, der von FireWire zurueckkommt
	DWORD   r[10];

	void Init(){ error=ERR_NO; cmdfw=0; r[0]=0; r[1]=0; r[2]=0; r[3]=0; r[4]=0; r[5]=0; r[6]=0; r[7]=0; r[8]=0; r[9]=0; }

//    virtual const char* Name() = 0;
};

