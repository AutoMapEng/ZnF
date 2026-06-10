#pragma once
#include "ZfsToolsAPI.h"

#include <memory>
#include <string>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QStringList>
SWITCH_WARNINGS_ON



class ZFScan;
class ZFSHeader;
class ZFTRMatrix;

namespace zfstools
{
    /**
    \brief This class simplifies getting and setting information int a ZFSHeader.

    The client does not have to know the indices of certain entries in the header but can rather use
    a simple getter function.

    Note that changes in the header are not written automatically to the file. You need to explicitly call
    writeChangesToFile() to do that.
    */
    class ZFSTOOLS_API  ZfsHeaderReaderWriter
    {
    private:
        ZFSHeader* notOwnedHeader;                  ///< scan header that is not owned by the ZFSHeaderReader
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::unique_ptr<ZFSHeader> ownedHeader;    ///< scan header that is owned by the ZFSHeaderReader
#pragma warning(pop)
        // used indices in the header
        static const int pyhsicalPixelIndex;
        static const int physicalLinesIndex;
        static const int registrationMatrixStartIndex;
        static const int registrationMethodFlagIndex;
        static const int subScanNamesEntryIndex;
        static const int scanPositionNameIndex;
        static const int maxRangeIndex;
        static const int minIntensityIndex;
        static const int maxIntensityIndex;

    public:
        ZfsHeaderReaderWriter( ZFScan* scan);         ///< ZfsHeaderReaderWriter does not take ownership of the scan
        ZfsHeaderReaderWriter( ZFSHeader* header);    ///< ZfsHeaderReaderWriter does not take ownership of the header
        ZfsHeaderReaderWriter( const std::wstring& fullScanFilename);    ///< This version opens the header from a file and closes it on destruction.

        bool writeChangesToFile();  ///< This must be called after setting header values.

        int getPhysicalScanPixel();
        int getPhysicalScanLines();

        double getMaximumRangeOfScanner();
        double getMinimumIntensityPercent();

        ZFTRMatrix getRegistrationMatrix();         ///< The values of the ZFScan parent class matrix.
        void setRegistrationMatrix( ZFTRMatrix matrix);

        int getRegistrationMethodFlag();           ///< Flags are defined in register_defines.h in the registermodul
        void setRegistrationMethodFlag( int flag);
        bool registrationMethodFlagExists();

        std::wstring getScanPositionName();
        void setScanPositionName( std::wstring positionName);

        QStringList getShortFilenamesOfSubScans();


    private:
        ZFSHeader* getHeader();
    };

}   // end namespace
