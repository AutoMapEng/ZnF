#pragma once
#include "../lrcsocketapi.h"

#include <list>
#include <string>
#include <memory>


#pragma warning(disable: 4251 )
// 4251 disable DLL Schnittstelle



class CUDPWriteImpl;


/// the abstroct basis class for writer
class LRCSOCKET_API CUDPWrite
{
    friend class CUDPWriteImpl;

public:
	CUDPWrite();
    virtual ~CUDPWrite();

    bool Create(int port, const char* ip = NULL);

    bool CanSendData() const;

    int			Port() const;
    const char* Ip() const ;
    long		CounterSend() const;

    bool IsRunning() const;
    int Error() const;

    uint64_t bytesWritten() const;

    typedef void(*cb_error_f)(int error);
    void setErrorCallback(cb_error_f f);

    /// wait until all things are send out
    void flush();

    /// check if there is something to send in the list
    virtual bool isEmpty() const = 0;

protected:
    virtual bool Write() = 0;
    std::unique_ptr<CUDPWriteImpl> impl;

};

//-------------------------------------------------------------------------------------------------------

class LRCSOCKET_API CUDPWriteBin : public CUDPWrite
{
public:
    CUDPWriteBin();
    ~CUDPWriteBin();

	bool SendData( const char* p1, int l1, const char* p2=NULL, int l2=0, const char* p3=NULL, int l3=0 );

    bool CpyStart( int n );
    void CpyAddHead();
    void Cpy( const char* d, int l );

    virtual bool isEmpty() const override;

protected:
    virtual bool Write() override;

    int sequenz{};
    char* pc{};
    int pcn{}, pcsz{};	///< size
    int pcb{};		///< package number
    int pcseq{};		///< max packages
};

//-------------------------------------------------------------------------------------------------------

#ifndef COLORREF
typedef unsigned long COLORREF;
#endif

/**
\brief Send strings over a udp connection out. Strings are buffered
*/
class LRCSOCKET_API CUDPWriteStr : public CUDPWrite
{
public:

    /// send string
    bool SendData( const std::string& );
    bool SendData( const char* );

    /// code the col as html string
	bool SendData( COLORREF col, const char* );

    virtual bool isEmpty() const override;

protected:
    typedef std::list <std::string> strlist_t; 
    strlist_t strlist; ///< the string that should be send out in the background thread

    virtual bool Write() override;
};


