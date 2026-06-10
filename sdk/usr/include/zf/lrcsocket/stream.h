#pragma once
/*
    Copyright (C) 2018 by Zoller & Froehlich Gmbh Germany
*/

#include "lrcsocketapi.h"
#include "lrcsocket_config.h"

#include <string>
#include <functional>
#include <memory>

#include "ports.h"
#include "../zfs/Math/zfvec.h"
#include "../zfs/pixel.h"

class ZFSFilter;
class ZFLinedataStream;
class ZFSHeader;
namespace zfs
{
    class FilterParams;
}


namespace lrcsocket
{
    class StreamImpl;


    /**
    \brief handels the data stream from one scanner
           Does not communicate with the scanner, just receive lines and return the lines and may store it.
           Can optinal store the stream to a zfs file
    */
    class LRCSOCKET_API Stream
    {
    public:
        Stream( int cache=500 ); ///< cache size should be in between 100 and 10000. Typically 200-500
        virtual ~Stream();

        enum err_t { ERR_OK, ERR_TYPNOTSUPPORTED, ERR_WRITING_FILE, ERR_CACHE_OVERRUN, ERR_EXCEPTION };

        /// connect and start internal thread for receiving datastream via UDP
        /// \param addr filter out stream with defined ip address
        bool connectUDP( int port= PORT_LINESTREAM_DEFAULT, const std::string& addr="" );

        /// connect to device via tcp interface for data stream
        /// scancommand needs additional param '-linestream tcpzip'
        /// need firmware 9.3.3 at least
        /// \param port Use typically 6105
        bool connectTCP(int port, const std::string& addr = "");

        /// obsolete / experimental
        /// use the old not longer supported xyz stream for older devices using tcp
        /// should not be used for 5016 or 9020 and newer devices. Use tcp instead
        /// \param port Use typically 6105
        /// \param addr filter out stream with defined ip address
        bool connectXYZ(int port, const std::string& addr = "");

        // outdated. Please use connectUDP instead
        bool connect(int port = PORT_LINESTREAM_DEFAULT, const std::string& addr = "") { return connectUDP(port, addr); }

        /// disconnect socket and stop thread
        void disconnect();

        /// if connected
        bool isConnected() const;

        // checks if receiving valid datas via checking for changes in stream
        // will be true after first received data
        bool isReceivingData();

        /// addr used for connect
        std::string connectedAddr() const;

        /// set a context for the callbacks
        void setContext(void* context);

        /// get the recommended scan param part required for streaming
        /// you have to be connected to get a string, otherwise empty
        /// example: ' -linestream tcpzip -ip 192.168.3.1'
        /// \param addr to ip addr where the scanner device should send the stream to. Should be the network addr of this pc.
        std::string getRecommendedScanParam( const std::string& addr ) const;

        // filter
        /// activate/deactivate filter. Keep settings
		void activateFilter(bool);
		bool isFilterActive() const;
        /// set new params used for next line
		void setFilter(const ZFSFilter&);
        void setFilter(const zfs::FilterParams& );
        void setFilter(const std::shared_ptr<zfs::FilterParams>);

        const ZFSFilter* getFilterParams() const;


        //
        // tools for storing datastream
        //

        /// set the size on which a new zfs file should be created
        void setSplitSize(uint64_t bytes);

        /// start recording the datastream into a zfs file
        ///!!!does not work with XYZ streams !!!
        bool startFile(const std::wstring& fileName);

        /// stop writing zfs file
        void closeFile();

        /**
        \brief force to switch to next file on next line
        \return false if no writer is active (no startFile is set)
        */
        bool switchFile();

        /**
        \brief enable or disable writing.
               disable will create a new part file
        */
        void writing(bool enable);

        /// return true if writing lines to file
        bool isWriting() const;

        void writingWithoutSwitch(bool enable);

        /**
        called bevore file will be created, you can change filename.
        Per default suggestion is returned.
        Return a empty string will stop recording
        This will be called for first and all following files
        For the first file, currentpart is 0
        */
        typedef std::function<std::wstring(void* context, const std::wstring& firstStartFilename, const std::wstring& suggestion, uint32_t currentpart)> fn_cb;
        void fileNameCallback(  fn_cb cb);


        /// called whenever a error is detected
        typedef std::function<void(void* context, err_t nr)> cbError_f;
        void errorCallback(cbError_f cb);

        /**
        \brief  called when a zfs-header is received. Afterwards the FilterParam callback is called.
                Be carefully if you want change values
        */
        typedef std::function<void(void* context, ZFSHeader& )> cbHeader_f;
        void headerCallback(cbHeader_f cb);

        /// allow to modify filter params preset by the zfs header
        typedef std::function<void(void* context, ZFSFilter&, const ZFSHeader&)> cbFilterParam_f;
        void filterParamCallback(cbFilterParam_f cb);


        /**
        \brief callback called if new line available
               !!! typically use XYZCallback instead !!!
               delayed by 3 lines. After receiving the 3. line you will get the first callback with filtered line if filter defined.
			   callback will be called async
        */
        struct LineData
        {
            uint32_t pixel {}; ///< number of pixel in the line
            const char* lineHeader {}; ///< pointer to the lineheader
            const char* linePixel {};  ///< pointer to the pixel itself (after lineheader)
            uint32_t lsz {};   ///< size of the full line inc. lineheader
            uint32_t lhsz {};  ///< line header size
            uint32_t bpp{}; ///< bytes per pixel
            bool calibrated {}; ///< wheter data are accurate or just inaccurate preview (fallback)
            bool filtered {};  ///< flag tells if line was filtered. Filter can only work is a zfs-header was received. activateFilter() can switch off/on filtering
            double refRf {}; ///< reference intensity if available [inc] added in 9.1.6
            double refRg {}; ///< reference range if available [meter] added in 9.1.6
            uint32_t minRf {}; ///< minimum intensity
            double eleOffsetDeg {}; ///< elevation offset
            double rgOffsetM {}; ///< referenze range offset
            std::wstring filterReport;
        };
        typedef std::function<void(void* context, const LineData& )> cbLine_f;
        void lineCallback(cbLine_f cb);

        /**
        \brief XYZ callback will be direct called after line callback
               return n x 3D points in local scanner coordinate system
               filtered out pixel will be set to zero
        */
        typedef zfs::PDataDetail stream_pdata_t;
        typedef std::function<void(const Stream& strm, void* context, const LineData&, const stream_pdata_t* pixels )> cbXYZ_f;
        void XYZCallback( cbXYZ_f cb);

        /// return pointer to underlaying object
        ZFLinedataStream* obj();
        const ZFLinedataStream* obj() const;

        /// return the current delay of the chache [lines]
		int delay() const;

        // internal test funtion
        void test(bool en);

        

        // some usefull statistic values
        // not all values are availablein all modes and on all devices
        struct Statistic
        {
            // status
            bool filterPossible{};
            bool recordPossible{};

            // stream
            std::string addr;   ///< address of the device who send the data
            bool valid{};   ///< is header available and calibrted
            int32_t datatype{};
            uint32_t nErrors{}; ///< counts errors like overrun
            uint32_t lastErrorCode{};
            int64_t missedLines{}; ///< number of missed lines
            int64_t trashed{}; ///< thrashed lines
            uint32_t reconnections{}; ///< tcp reconnections
            uint32_t nHeaderReceived{}; ///< number of zfs header received
            uint32_t lines{}; ///< current line
            uint64_t pixel{}; ///< sum of all pixel received
            uint64_t nCB{};
            uint32_t flagsOfLineheader{}; ///< the flags of the lineheader
            double elevationOffsetDeg{};
            uint32_t lineHeaderTime{}; ///< [ms]
            uint32_t counterL{}; ///< horz counter
            uint32_t counter1{}; ///< typically PPS
            uint32_t counter2{};
            uint32_t counter3{}; ///< placeholder

            // writing to file stuff
            bool started{}; ///< started and waiting for lines to store
            bool writingToFile{}; ///<
            std::wstring currentFile; ///< full file name
            uint32_t linesWrittenToFile{}; ///<
            double mbCurrentFile{}; ///< megabytes written to file. total of all parts
            double mbAllParts{}; ///< megabytes written to current part
            uint32_t currentPart{}; ///< current part

            // performance
            double lps{}; ///< lines per second (calculated of data stream)
            double datarate{};
            uint32_t cacheFree{}; ///< current used
            uint32_t cacheSize{}; ///< cache size for filter and writing
            double perfXYZ{}; ///< performance calculation [ms]
            double perfFilter{}; ///< performance filter [ms]
            double perfBackground{}; ///< performance background processing [ms]

            uint64_t available{}; ///< see pixel available definition

        };
        Statistic getStatistic() const;
        void resetStatistic();
        void resetcurrentline();
       

    protected:
        StreamImpl* impl = nullptr; ///< local
    };

} // end namespace


