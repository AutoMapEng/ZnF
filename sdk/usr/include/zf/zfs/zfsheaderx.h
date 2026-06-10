#pragma once

/**
	Extension of functionality of zfsheader class
*/

#include "zfsapi.h"
#include "compiler.h"

#include <tuple>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <list>

#include "zfsheader.h"
#include "zfvector.h"
#include "zfsheader_index_basic.h"

class ZFTRMatrix;

namespace zfs
{

    /**
    \brief return the number of pixel for 360deg
    */
    int ZFS_API getPixel360deg(const ZFSHeader& h);

    /**
    \brief return subsample used = encoder / pixel360deg
    */
    double ZFS_API getPixelSubsample(const ZFSHeader& h);

    /**
     * @brief Determines whether scanner would deliver a selection over UDP
     *
     * Via UDP is normally only the full line transfered. But if streaming of
     * compressed lines is enabled only the selection get's compressed and
     * transfered.
     *
     * @return true if scanner whould have delivered compressed selection
     */
    bool ZFS_API streamCompressedSelection(const ZFSHeader &h);

    //
    // Note:
    // ZFSHeaderX is impossible to implement const correct (ZFSHeader meber needs to be const)
    // But all members are equally well as free functions operating on (const) ZFSHeader&
    //


    /**
    \brief high level extension of zfs-header access
    */
	class ZFS_API ZFSHeaderX
	{
    protected:
		const ZFSHeader& h;

	public:
        ZFSHeaderX( const ZFSHeader& head )  : h(head) {}

		int getPhysPixel() const { return h.getLong(ZFSHEADIDX_PIXEL,0); }
		int getPhysLines() const { return h.getLong(ZFSHEADIDX_LINES,0); }
		ZFTRMatrix getRegistration(int index = ZFSHEADIDX_REGISTER_EXTERN) const;

        int getType() const { return h.GetLong(1, 0) & 0x0fffffff; }

        /**
         * @brief Get the attached files in the header.
         *        The filenames are returned "as is", no extension or path is removed/added.
         *
         * @param fileList list to be filled with filenames
         */
         void getAttachedFiles(std::list<std::wstring>& fileList) const;

        /**
         * @brief Returns the first index which is empty between 51000 - 51020.
         *
         * @return the free index or -1 if no more free fields are available
         */
        int getFreeIndexForAttachedFile() const;

        /**
         * @brief Looks up the given filename part. Currently it is just checked if the part is found.
         *
         * @param filePart the part to search
         *
         * @return an index between 51000 - 51020 or -1 if the part is not contained
         */
        int findAttachedFile(const std::wstring& filePart, const std::wstring& noMatchPart = L"") const;

        /**
        \brief read out sensor from the zfs-header
        */
        std::vector< std::pair<int, double> > readSensors(int startIndex) const;

        ///< return the scan command used for scanning
        std::string getScanCommandUtf8() const;
        std::wstring getScanCommandUtf16() const;

        std::wstring getCommentUtf16() const;

        /// the serial number of the device
        std::string getSerial() const;

        /**
        \brief return the number of pixel for 360deg
        */
        int getPixel360deg() const { return zfs::getPixel360deg(h); }

        /**
        \brief return subsample used = encoder / pixel360deg
        */
        double getPixelSubsample() const { return zfs::getPixelSubsample(h); }

        /**
        \brief Minimum (first) and maximum (second) intensity
        */
        std::pair<int32_t,int32_t> getMinMaxIntensity() const;

        /// minimul intensity as percent 0..100
        double getMinRfPercent();

        /**
        \brief Minimum range of system [meter]
        */
        double getMinRangeMeter() const;

        /**
        \brief Max range of system [meter]
        */
        double getMaxRangeMeter() const;

        /**
        * @brief Convinience method to read out navigation values from the scan header.
        *
        * @param rotation    the rotation measured by the compass (out)
        * @param height      the barometer height (out)
        * @param upsideDown  flag, if the scanner was upside down (out)
        *
        * @return true, if the values where present
        */
        bool getIMUSensorValues(double& rotation, double& height, bool& upsideDown) const;
        bool getIMUSensorValues(ZFVec3d& rotation, double& height, bool& upsideDown) const;



        /**
        \brief Date and time scan was taken
        \return as a string
        */
        std::string scanCaptureDateTime(const std::string& format = "%H:%M:%S %d.%m.%Y") const;


        /**
        \brief read out all masks added to this scan
        */
        std::vector< std::wstring> getMaskEntries() const;
        std::map<std::wstring, int> getMaskColors() const;


        // return a list of scan errors/earnings
        std::vector<std::wstring> getScanErrors() const;

    };




	class ZFS_API ZFSHeaderXSet
	{
	protected:
		ZFSHeader& h;

	public:
		ZFSHeaderXSet(ZFSHeader& head) : h(head) {}

		bool setRegistration(const ZFTRMatrix& mat, int index = ZFSHEADIDX_REGISTER_EXTERN, bool checkValues = false);



		/**
		\brief Remove the mask entries from the header
		\return number of maskes removed
		*/
		int removeAllMaskEntries();

	};

} // end namespace zfs


