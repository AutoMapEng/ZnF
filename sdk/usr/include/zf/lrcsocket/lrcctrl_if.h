#pragma once

#include "lrcsocketapi.h"
#include "lrc_def.h"

#include <string>
#include <vector>
#include <list>
#include <utility>

#include "Client.h"
#include "ports.h"

#include <zfcore/version.h>


//########################################################################################################

/*
    Implements a simple TCP/IP ASC Command interface to the scannerdevice

    All the commands are send as utf-8 with a cr at the end.

    For a more complex interface you have to implement your own handler
*/



#define LRCCTRL_CMD_TIMEOUT 200		// default timeout for command. !! some commands take longer

/*
    class for controlling device over a tcp/ip connection
*/

namespace boost { class thread; class mutex; }

#define MAX_HOLD_MESSAGES	20	///< wie viele zurueckliegende messages behalten werden


/**
@brief Stores the messages got from the scanner
       holds the last n messages
*/
class LRCSOCKET_API CmdResult
{
public:
    std::string	result;	///< utf8 result string
    double pctime{};		///< the pc time when get result
    bool handeld{};		///< wenn sie erkannt wurde

    CmdResult() = default;
    CmdResult(const std::string& add);
};



class LRCSOCKET_API LRCInterface : public ZFTCPClient
{
protected:
    void OnReadParse(const std::string& s);

    virtual void OnReadXML(const std::string& ) override;
	virtual void OnClosed( int ) override;

    /**
     * @brief Try to start a connection to the device specified with the connection parameters.
     */
    bool startConnection();
    void DetectSub();
    void ClrResults();
    void AddResult(const std::string&);

public:
    LRCInterface();
    virtual ~LRCInterface();

    enum typ_t { UNKNOWN, I5003, I5006, I5010, P9000, I5016, I5024 }; // protokol/scanner typ (series). Single lara system has same typ as the Imager series
    enum sub_t { SUB_UNKNOWN, SUB_BASIC, SUB_C, SUB_X, SUB_N }; // SUBTYPES
    enum support_t { SUPPORT_ICAM = 1, SUPPORT_NAV = 2, SUPPORT_DYNCOMP = 4 };

    /**
    *   Connect to device.
    *
    *   Address resolution is done using getaddrinfo(), which resolves IPv4 and
    *   IPv6 addresses.
    *
    *   \param server => Scanner IP as string or hostname
    *   \param port   => Port number to connect to. Default = 6100
    *   \retrun true if scanner could be connected
    */
    bool Connect(const std::string& ip, int port = LRC_PORT);

    /**
    *   \brief detect device typ,serial,...   must be called after Connect()
    *   \param usexml whether to switch to xml protokoll or not. XML protokol is better supported.
    *   \return true if any known device is detected
    */
    bool Detect(bool usexml = true, unsigned int to = LRCCTRL_CMD_TIMEOUT, const std::string& pin = {});

    /**
    *   Disconnect scanner.
    *   \param waitStopped => obsolete
    *   \return true if scanner is disconnected.
    */
    bool Disconnect(bool waitStopped = false);


    /**
    *   Check is scanner is connected.
    *   \return true is scanner is connected.
    */
    bool IsConnected() const
    {
        if (isConnected)
            return true;
        return false;
    }


    /**
    *   Check if scanner is connected. 
    *   For Scanner type I5003 if device is connected and recogniced by lrccserver
    *   \param bStatusOnly => obsolete
    *   \return True if scanner is connected.
    */
    bool IsOk(bool bStatusOnly = false); 

    /// return the connection address
    std::string Addr() const { return connectionIP; }

    /**
    *   internal access name of device
    */
    int Serie() const { return serie; }

    /**
    *   Scanner Type
    *   return Type of connected Scanner. (UNKNOWN, I5003, I5006, I5010, P9000, I5016, ...)
    */
    typ_t Typ() const { return typ; }  // device protokol/scanner typ see enum

    /**
    *   Suptype of the Scanner
    *   \return Suptype of Scanner ( SUB_UNKNOWN, SUB_BASIC, SUB_C = 5010C, SUB_X = 5010X, SUB_N =5010N )
    */
    sub_t SubTyp() const { return subtyp; }

    /**
    *   Device serial number
    */
    const char* SerialNo() const { return serial.c_str(); }
    /**
    *   Firmware version of the connected scanner.
    */
    const char* Firmware() const { return firmware.c_str(); }

    /**
    *    Model name version ( 5006/5010/5010X/5010C/5010N/5016/... )
    */
    const char* Model() const { return model.c_str(); } 

    /**
    *   Flag if comand return is available.
    */
    bool HasReturn() const { return rd; }

    /**
    *   Send a command to the scanner. Get result with Result()
    *   \param s => Command string
    *   \param to => Maximum time to wait for the result. to=0 it return immediately
    *   \return true if a result is availabel
    */
    bool Command(const char* s, unsigned int to = LRCCTRL_CMD_TIMEOUT); 

    // exception version
    bool CommandX(const char* s, unsigned int to = LRCCTRL_CMD_TIMEOUT);

    /**
    *   Send a command and receive result.
    *   \param s => Command string
    *   \param to => Maximum time to wait for the result. to=0 it return immediately
    *   \return Result of the command
    */
    const std::string& Cmd(const char* s, unsigned int to = LRCCTRL_CMD_TIMEOUT);

    /**
    *   Send a command and receive result.
    *   \param s => Command string
    *   \param to => Maximum time to wait for the result. to=0 it return immediately
    *   \return Result of the command
    */
    const std::string& Cmd(const std::string& s, unsigned int to = LRCCTRL_CMD_TIMEOUT);

    /**
    *   Wait for (next) result if previous result is not what you expected or you send command without waiting.
    *   \param trash => trash last result before waiting
    *   \param to => Maximum time to wait for the result. to=0 it return immediately
    *   \return true if a result is availabel
    */
    bool WaitResult(bool trash, unsigned int to = LRCCTRL_CMD_TIMEOUT); 

    /**
    *   Returns the result String of the last comand.
    */
    const char* Result() const { return result.c_str(); }

    /**
    *   Returns the value of the passed config entry.
    *   \param value => Name of the config entry.
    *   \param def => Default value is the entry is not available on the scanner.
    *   \return value of the config entry or default if the entry is not available.
    */
    std::string Ask4Config(const std::string& value, const std::string& def);
    int Ask4Config( const std::string& index, int def );
    double Ask4Config( const std::string& index, double def );
    
    /**
    *   Check if the passed config entry is available
    *   \param index => Name of the config entry.
    *   \return true if the value is available
    */
    bool Ask4ConfigExist(const std::string& index);


    /**
    *   Wait until the return has been received completely.
    */
    void WaitParsingFin();


    /**
    @brief check if command is in the last result messages
    @param command = Command whose answer is searched
    @param maxAge = maximum age for the answer
    \return true if the result message is found
    */
    bool searchMessage(const std::string& command, double maxAge = 200);

    enum msgtyp_t { MSGTYP_UNDEF, MSGTYP_WARN = 1, MSGTYP_ERROR, MSGTYP_FATAL, MSGTYP_INFO, MSGTYP_YESNO };


    /**
    *   Called by own thread. Implementation must be tread save. Called after successfull connection.
    */
    virtual void OnConnect() {}	

    /**
    *   Called if device reports error msg and info may null pointer.
    */
    virtual void OnMessagebox(int nr, const char*, msgtyp_t act, const char* info) {} 

    /**
    *   Called if device sends an eventmessage. Only on Imager5006. String is a xml string.
    */
    virtual void OnEvent(const char*) {} 

    // lowlevel
    /**
    *   Called for every received string ended with cr.
    */
    virtual void OnString(const std::string& s) { }

    // misc

    /**
    \brief Return status bit mask see STATUS_...
    \param bFromBuffer false=send command to get latest status, otherwise the last status is just returned
    \return last status if command failed or zero if not connected
    */
    int getStatus(bool bFromBuffer=false );

    /**
    *   Logging out of coming messages.
    */
    void EnableLog(bool en = true) { bLog = en; }

    /**
    *   Returns the loged messages.
    */    
    const std::string& Log() const { return log; }

    // high level

    /**
    *   Ignor the error and go on.
    */
    void Ignore();

    /**
    *   Abort progress.
    */
    void Abort();

    /// not implemented jet
    void Retry();

    /**
    *   Stop scanning.
    */
    void Stop();

    enum
    {
        STATUS_ID = (1 << 0),	///< must be set (0001h)
        STATUS_STREAM = (1 << 1),	///< datastreaming is running (0002h)
        STATUS_PRG = (1 << 2),	///< scanprogress is running (0004h)
        STATUS_ERR = (1 << 3),	///< error while scanning (0008h)
        STATUS_WR = (1 << 4),	///< writing to disk (0010h)
        STATUS_PIN = (1 << 5),	///< device protected via pin (0020h)
        STATUS_LOCK = (1 << 6),	///< device locked via login (0040h)
        STATUS_SPACE = (1 << 7),	///< run out of space (0080h)
        STATUS_NOTREADY = (1 << 8),	///< booting or hardwareerror, ... (0100h)
        STATUS_BATCH = (1 << 9),	///< batch active. There are jobs in the batch list (0200h)
        STATUS_PAUSE = (1 << 10),     ///< scan pause active (typical for imager) (0400h)
        STATUS_DATA_INVALID = (1 << 11), ///< device sends data, but they are invalid. For example profiler needs several seconds until data are valid (0800h)
        STATUS_CAMERA_JOB_ATTACHED = (1 << 12), ///< scanjob with camera attached. Can be changed while scan job is running
        STATUS_CAMERA_CAPTURING = (1 << 13), ///< true while capturing images and writing the image-container
        STATUS_POST = (1 << 14), ///< after creating scan flag STATUS_WR is done and postprozessing is running
        STATUS_PRE = (1 << 15), ///< startupsequenze
        STATUS_USERABORT = (1 << 16), ///< user aborted scan or panorama
    };

    /**
    \brief get scannerstatus via command "status. if 0 returned, call failed. please try once more   
           new in 8.6
    \return bit mask, see STATUS_... or zero if failed
    */
    int GetStatusViaCmd(); 

    /** calls command statusbar
    * \return systemstatus as integer and as string
    */
    std::pair<int,std::string> GetSystemStatus(); 




    void resetResult();
    bool hasResult() const;
    std::string getResult() const;
    bool send(const std::string& s);



    // helper functions for parsing xml
    /**
    *   Simple parser for the result recived with Cmd(). Without validation check. Returns the text value for the given token. (opsolete)
    *   Example:
    *   std::string xmlTxt = "<xml><ret cmd=\"config\" err=\"0\" msg=\"OK\"><config idx=\"locale\" typ=\"string\" mod=\"get\">en</config></ret></xml>";
    *   std::string res = LRCInterface::ParseXMLOnly( xmlTxt,"config");
    *   res == "en"
    *   \param in => XML Result string from Scanner.
    *   \param token => XML token to look for.
    *   \return Text of the token. Or "" if the value was not found.
    */
    static const std::wstring Parse(const std::wstring& s, const wchar_t* t);
    static const std::string Parse(const std::string& in, const char* token); // parse result recived with Cmd()
    /**
    *   Simple parser for the result recived with Cmd(). Without validation check. Returns the given attribute value for the given token.
    *   Example:
    *   std::string xmlTxt = "<xml><ret cmd=\"config\" err=\"0\" msg=\"OK\"><config idx=\"locale\" typ=\"string\" mod=\"get\">en</config></ret></xml>";
    *   std::string res = LRCInterface::ParseXMLOnly( xmlTxt,"config", "idx");
    *   res == "locale"
    *   \param in => XML Result string from Scanner.
    *   \param token => XML token to look for.
    *   \param attr => XML attribute to look for.
    *   \return Value of the attribute. Or "" if the value was not found.
    */
    static const std::wstring ParseXMLAttrOnly(const std::wstring& in, const wchar_t* token, const wchar_t* attr); // parse result recived with Cmd()
    static const std::string ParseXMLAttrOnly( const std::string& in, const char* token, const char* attr ); // parse result recived with Cmd()
    /**
    *   Simple parser for the result recived with Cmd(). Without validation check. Returns the text value for the given token.
    *   Example: 
    *   std::string xmlTxt = "<xml><ret cmd=\"config\" err=\"0\" msg=\"OK\"><config idx=\"locale\" typ=\"string\" mod=\"get\">en</config></ret></xml>";
    *   std::string res = LRCInterface::ParseXMLOnly( xmlTxt,"config");
    *   res == "en"
    *   \param in => XML Result string from Scanner.
    *   \param token => XML token to look for.
    *   \return Text of the token. Or "" if the value was not found.
    */
    static const std::string ParseXMLOnly(const std::string& in, const char* token); // parse result recived with Cmd()
    static const std::wstring ParseXMLOnly( const std::wstring& in, const wchar_t* token ); // parse result recived with Cmd()


    int closeErrorCode() const { return m_closeError; }

protected:
    bool isConnected{};
    bool monitor{};

    int serie{};              ///< scanner serie
    typ_t typ{};				///< grundtyp (Scannerserie)
    sub_t subtyp{};				///< z.B SUB_COLOR, ... ansonsten SUB_BASIC oder SUB_UNKNOWN wenn nicht ermittelt werden kann
    int support{};			///< see SUPPORT_...
    std::string serial;		///< serial number
    std::string firmware;	///< firmware version string
    zf::Version firmwareVersion;
    std::string model;		///< Model name

    volatile bool rd{};
    std::string result;
    std::list<CmdResult> messages;		///< the last received messages from the scanner
    int resultIdx{};
    bool bLog{};
    std::string log;
//    char* work{};
//    int workPtr{};
    char* sIn{};
    int sInSize{};
    int sInWr{};
    int sInRd{};
    int m_closeError{}; ///< errorcode from socket why socket was closed

private:
    int m_nStatus{};

    std::string connectionIP;
    int connectionPort{};
    bool bParsing{};

};


namespace lrcsocket
{
    // interprete the bits and build a string
    LRCSOCKET_API std::string statusbyte2string(int byte);
}