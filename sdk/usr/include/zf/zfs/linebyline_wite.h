#pragma once

#include "zfbase.h"
#include "compiler.h"

#include <string>
#include <functional>
#include <mutex>

#include "../Common/inttypes_.h"

class ZFLine;
class ZFLineAccess;
class ZFLineInfo;
class ZFSHeader;

namespace zf { class ZFFile; }


/**
 * @brief ZFSLineByLine writer
 *
 * Create a ZFSFile and append lines.
 *
 * @note Subsample is only supported with source lines of type 215 and 214!
 */
class  ZFS_API ZFLineByLineWrite : public ZFBase
{
public:

    /**
     * @brief Create new ZFSLineByLine-Writer
     *
     */
    ZFLineByLineWrite();
    ~ZFLineByLineWrite();

    int getSize();
#ifdef WIN64
    int64_t getSize64();
#endif

    /**
    * @param name    Name of target file
    * @param h       Header copied to target file (may be modified)
    * @param pixsubs Pixel subsample value to use >= 1 (1 -> no subsample)
    */
    virtual bool create(const std::wstring& name, const ZFSHeader* h, int pixsubs = 1, bool deletePartsBefore = true, int forceCompressionType = 0);


	/// wheather scan file is open and can be written
	bool isOk() const { return ok; }

	/// close the scan file
	virtual void close();


    void setHeaderValueLong( int idx, long in );
    void setHeaderValueString( int idx, const wchar_t* in );
    void setHeaderValueDouble( int idx, double in );

	/// write the header to the file
    void writeHeader();

    std::string getLastError() const { return lastError; }

    /**
    *   Copy  a new ZFLineAccess object of the raw data and information stored in info*/
    static ZFLineAccess* copyLine( ZFLine* line, ZFLineInfo& info );

    /** create a new ZFLineAccess object of the raw data and information stored in info*/
    static ZFLineAccess* createLine( ZFLineInfo* info );

    virtual bool writeLine( ZFLine* l, ZFLineInfo& info );
    virtual bool write( ZFLineAccess* l );

    // delete files like .pos
    void deleteTempFiles(bool deletePartsAsWell=true);


protected:
	std::string lastError;	    ///< Letzter aufgetetene Fehler
    zf::ZFFile* zfsFile{};  ///< Ausgabedatei
	std::wstring fileName;	    ///< Name der Ausgabedatei
    mutable std::mutex mutFileName;
	int lineNr{};				    ///< aktuelle Zeilennummer
	ZFSHeader* head{};		    ///< Zeiger auf FileHeader
	int m_pixsubs{1};        ///< Subsample for pixel data
    int m_compression{};     ///< the compression which sould be used

    double m_fst_henc{};          ///< First Horz. Encoder written
	double m_lst_henc{};          ///< Lastr Horz. Encoder written
	double m_cov_henc{};          ///< Covered Horz. Encoder range

	bool ok{false};

protected:
	virtual bool openFile(const std::wstring& name, const ZFSHeader* h, int forceCompressionType = 0);

	bool subsample_215(ZFLineAccess * in_line);
	bool subsample_214(ZFLineAccess * in_line);

private:
    ZFLineByLineWrite( ZFLineByLineWrite& ): ZFBase(), m_pixsubs(0) {}
    const ZFLineByLineWrite& operator=( ZFLineByLineWrite& ) { return *this; }

};

//#############################################################################################################################################
namespace zfs
{
    /**
    \brief write a zfs file and split file
    */
    class ZFS_API ZFLineByLineWriteSplitted : public ZFLineByLineWrite
    {
    public:
        ZFLineByLineWriteSplitted();
        
        /**
        \param forceCompressionType forces to change the compressor. 0=let it as it is
        */
        virtual bool create(const std::wstring& name, const ZFSHeader* h, int pixsubs = 1, bool deletePartsBefore=true, int forceCompressionType=0 ) override;

        virtual void close() override;

        /**
        \brief set the size on which a new file/part shold be created. Minimum 1024
        \return the size in bytes used
        */
        uint64_t setSplitSize(uint64_t splitSize);
        uint64_t setSplitSizeMB(uint64_t splitSizeMB) { return setSplitSize(splitSizeMB*1024*1024); }

        /// callback for progress. return false for user abort
        virtual bool onProgress(int line, int part) { return true; }

        /// called before current file will be closed and next file will be started. zfs-header can be modified and will be stored
        virtual void onCloseCurrentFile(ZFSHeader*) {}

        /**
        \brief write the line to disk
        \param lastLineWritten number to store and get access via lastLineWritten()
        */
        virtual bool write(ZFLineAccess* l, uint32_t lastLineWritten=0 );

        /**
        \brief writes the buffer to the file without any conversation
               just the size of the buffer will be written at the beginning as well
               Increment the lines and bytes as well
        */
        virtual bool writeRawLine(const char* ptr, int sz);

        uint64_t bytesWrittenInAllParts() const { return bytesAllParts; }
        uint64_t bytesWrittenInCurrentPart() const { return bytesCurrentPart; }
        uint64_t partsWritten() const;
        uint64_t linesWrittenAllParts() const { return linesAllParts; }
        uint32_t currentPartNr() const { return currentPart; }

        /// get the last line written
        uint32_t lastLineWritten() const { return nrLastLineWritten; }
        /// get the last line number of lineheader written
        uint32_t lastLineNumberWritten() const { return nrLastLineNumerWritten; }

        /// tell to switch file
        void switchPartOnNextWrite();

        /// change file name on the fly. first filename can be changed when currentPart is zero
        typedef std::function<std::wstring(void* context, const std::wstring& firstStartFilename, const std::wstring& suggestion, uint32_t currentpart)> fn_cb;
        void filenameCallback(fn_cb, void* constext=nullptr);

        std::wstring currentFileName() const;

    protected:
        virtual bool openFile(const std::wstring& name, const ZFSHeader* h, int forceCompressionType = 0) override;

        /// check wheter a fileswitching should be done (return true)
        bool check4FileSwitching();

        /// switch file to next part and do all necessary tasks like create links in the zfs-header
        bool switchFile();

    protected:
        bool switchPart{ false };
        std::wstring firstFilename;
        uint64_t splitSize{ 2000000000 };
        uint32_t currentPart{0};
        uint64_t bytesCurrentPart{ 0 };
        uint64_t bytesAllParts{ 0 };
        uint32_t linesCurrentPart{ 0 };
        uint32_t linesAllParts{ 0 };
        uint32_t nrLastLineWritten{ 0 }; ///< the last line number stored in the file
        uint32_t nrLastLineNumerWritten{ 0 }; ///< the last line number of the lineheader stored in the file
        fn_cb fncb;
        void* context{ nullptr }; ///< context for callback cbFileName
    };// end class

}// end namespace