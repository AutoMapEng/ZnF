#pragma once

#include <string>
#include <memory>

#include "lrcsocketapi.h"
#include "devs.h"
#include "platform/zfsocket.h"
#include "lrc_def.h"
#include "udpstream.h"
#include "ports.h"

class ZFSHeader; // from zfs.dll library

namespace std
{
	class thread;
}
struct SDataHeader;

namespace lrcsocket
{
	/**
	\brief Receives lines over UDP socket.
		Can support multiple streams from different scanners, dev struct will tell from which scanner data is.
		Just for intarnal usage only
	*/
	class LRCSOCKET_API UDPLinedataStream
	{
	public:
		UDPLinedataStream();
		virtual ~UDPLinedataStream();


		/// connect to port and start thread
		virtual bool Connect(int port = PORT_LINESTREAM_DEFAULT, const std::string& addr="");
		/// close socket and stop thread
		virtual bool Disconnect(bool waitThread = true);
		/// wheter is connected
		bool IsConnected() const { return bConnected; }
		// check if thread is running
		bool IsThreadRunning() const { return !bTreadStopped; }

		/// called if connection is established
		virtual void OnConnected() {}

		/// get the underlying socket interface
		zfsocket getSock() const;

		/// increase buffersize for udp
#ifdef _WINDOWS
		bool setSockOptIncreaseBuffer(zfsocket);
#endif

		/// disable datastream, it will receive stream but not send it further
		void Freeze(bool f) { bFreeze = f; }

		/// get freeze status
		bool isFreezed() const { return bFreeze; }


		// lowlevel
		/**
		\brief receive full raw line
		\param dev   information aboud scanner and
		\param in    memory from socket
		\param lri   some information about scanner
		\param head  the line including the transfer header
		\param line  the line inclusive the lineheader
		\param pixel number of pixel in the line
		\param lsz   the size of the line inclusive lineheader
		*/
		virtual void OnRAWData(const ZFStreamDeviceInfo& dev, const char* in, const LineRawInfo& lri, const char* head, const char* line, int pixel, int lsz) = 0;

		/// receive a fileheader at start of scan
		virtual void OnRAWFileHeaderBuf(const ZFStreamDeviceInfo&, const char* in, const char* h, int size);
        virtual void OnRAWFileHeader(const ZFStreamDeviceInfo&, const char* in, const ZFSHeader& ) = 0;

		/// receive a command string
		virtual void OnRAWCmd(const ZFStreamDeviceInfo&, const char*) = 0;

		// detects if a new scan is started
		virtual void OnNewScanDetected() {}

		int getLineheadersize() const { return lineHeaderSize; }

	protected:
		volatile bool bTreadStopped = true;
		volatile bool bKill = false;
		bool bFreeze = false; // disable datastream OnData handling

		// valid for the current package
		std::string addr; ///< address if defined
		int typ = 0;		// datatyp 1 = line
		int ver = 0;		// version 1 = 5003 2=5006 3=5010
		double angle = 0.;	// elevation angle (deg)
		int pixel = 0;		// per Line
		int lineHeaderSize = 0; // bytes
		int bytesPerPixel = 0;	// bytes per pixel used

		volatile bool bConnected = false;

		static zf::UdpStream udp_raw_stream;
		
		ZFStreamDeviceInfo dev;

    private:
		void processBuffer(const std::vector<char>& szBufferIn, int iLen);
		void processLineBuffer(ZFStreamDeviceInfo&, const SDataHeader* pHead, const std::vector<char>& , std::vector<char> &tmpbuffer);
	};

}
