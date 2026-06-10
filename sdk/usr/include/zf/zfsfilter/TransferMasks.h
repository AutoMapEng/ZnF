#pragma once

#include <string>

#include "filterapi.h"
#include <cstdint>

class ZFScanZFS;
class ZFSLinebyline;

namespace zfsfilter
{
    // =================================================================================================================
    /** @class TransferMasks
      * @brief class for transfering masks from one file to another file
      * @author Juergen Holzner
      */
    class FILTERDLL_API TransferMasks
    {
    public:

        enum class TM_OPTION_t:uint8_t { TM_UNSET = 0, TM_TO_GIVEN_OUTPUT_SUBDIR, TM_TO_GIVEN_OUTPUT_FILE };

        // -- constructors
        TransferMasks();
        TransferMasks(const std::wstring& aInputFile, const std::wstring& aOutputFile, const TM_OPTION_t& aTMOption, const bool& a_bDeleteMaskFromOutputFile = false);
        TransferMasks(ZFScanZFS* const aInputScan, const std::wstring& aOutputFile, const TM_OPTION_t& aTMOption, const bool& a_bDeleteMaskFromOutputFile = false);
        ~TransferMasks();

        // -- public member functions
        void Initialize(const std::wstring& aInputFile, const std::wstring& aOutputFile, const TM_OPTION_t& aTMOption = TM_OPTION_t::TM_UNSET, const bool& a_bDeleteMaskFromOutputFile = false);
        void Initialize(ZFScanZFS* aInputScan, const std::wstring& aOutputFile, const TM_OPTION_t& aTMOption = TM_OPTION_t::TM_UNSET, const bool& a_bDeleteMaskFromOutputFile = false);

        // -- main worker function
        void  transferMasks();

        // -- getter
        bool WriteMasksOK()     const;
        bool AddMaskOK()        const;
        bool InputFileOpenOK()  const;
        bool OutputFileOpenOK() const;
        bool TransferMasksOK()  const;

    private:
        // -- private member variables
        bool         m_bWriteMasksOK;
        bool         m_bAddMasksOK;
        bool         m_bExternalOpen;
        bool         m_bInputFileOpenOK;
        bool         m_bOutputFileOpenOK;
        bool         m_bTransferMasksOK;
        bool         m_bDeleteAllMasksFromOutputFile;

        TM_OPTION_t  m_TMOption;

        std::wstring m_InputFile;
        std::wstring m_OutputFile;

        ZFScanZFS*     m_readZFS_ZFScan;
        ZFSLinebyline* m_writeZFS_LBL;

        // -- private member functions
        void  basic_initialization();
        // -- common initialization and eval of given options
        void  common_output_initialization();
        // -- close file function
        void  close_files();
        // -- construct output file name
        const std::wstring generate_output_file_name(const std::wstring& aInputFile, const std::wstring& aSubDirName);
    };

    // =================================================================================================================
    // Test Transfermask Class
    // =================================================================================================================
//    void FILTERDLL_API TransferMasksTest();
    // =================================================================================================================
} // zfsfilter
