#pragma once

#include <map>
#include <list>

#include "ZfsPrjBasic.h"
#include "xml.h"

#include <zftools/undo.h>

#define XML_MATRIX_STR	 "Matrix"
#define XML_INT_MATRIX_STR	 "InternMatrix"
#define UNDO_RENAME_SCAN L"UndoRenameScan"

namespace tinyxml2 {
    class XMLNode;
}

class ZFTRMatrix;
class ZFProject;
class ZFProjectItemScan;

namespace zfprj {

//#############################################################################

    class ZFSPRJ_API UndoRenameScan : public zftools::BaseUndoItem {

        std::map<std::wstring, std::wstring> renamedScans;
        std::map<std::wstring, std::wstring> renamedViewpoints;

    public:
        UndoRenameScan();
        UndoRenameScan(
            std::map<std::wstring, std::wstring> renamedScans,
            std::map<std::wstring, std::wstring> renamedViewpoints);

        virtual ~UndoRenameScan();

        virtual std::wstring getId() override { return UNDO_RENAME_SCAN; }
        virtual std::wstring getMessage() override;
        virtual void getUndoAttributes(zftools::undo_attributes_t& attributes) override;
        virtual void setUndoAttributes(zftools::undo_attributes_t& attributes) override;
        virtual void undoItem() override;
        virtual void afterUndo(std::list<int>& actions) override;

        static std::shared_ptr<UndoItem> Create() { 
            return std::make_shared<UndoRenameScan>();
        }
    };


//############################################################################

    ZFSPRJ_API  tinyxml2::XMLElement* deleteMatrixFromNode( tinyxml2::XMLNode* n,
        const std::wstring & matrixName, 
        bool markAsDeleted = false);

    ZFSPRJ_API void writeMatrixToNode(double* m,  tinyxml2::XMLNode* n, 
        const char* name = XML_MATRIX_STR, 
        const std::wstring& matrixName = L"", bool deleted = false);
    ZFSPRJ_API void writeMatrixToNode(ZFTRMatrix& m,  tinyxml2::XMLNode* n,
        const char* name = XML_MATRIX_STR, 
        const std::wstring& matrixName = L"", bool deleted = false);

    ZFSPRJ_API bool readMatrixFromNode( tinyxml2::XMLNode* n, ZFTRMatrix& m);

    ZFSPRJ_API bool readMatrixFromNode( tinyxml2::XMLNode* n, ZFTRMatrix& m,
        const char* nodeName);

    ZFSPRJ_API bool readMatrixFromNode( tinyxml2::XMLNode* n, ZFTRMatrix& m, 
        bool& deleted,
        std::wstring& matrixName);
    ZFSPRJ_API bool readMatrixFromNode( tinyxml2::XMLNode* n, ZFTRMatrix& m, 
        const char* name, 
        bool& deleted,
        std::wstring& matrixName);

//############################################################################
    class ZFSPRJ_API RenameResult {
    public:
        std::map<std::wstring, std::wstring> renamedFiles;
        std::list<std::wstring> errors;        
        std::list<std::wstring> warnings;
    };

    ZFSPRJ_API bool renameScan(
        ZFProject* proj,
        const std::wstring& oldScanName, 
        const std::wstring& newScanName,
        const std::wstring& newViewposName = L"",
        RenameResult* result = nullptr);

    /**
    *   Umbenennen der erstellten Datenbanken fuer einen Scan. Der Scan muss schon umbenannt sein.
    */
    void renameScanInZFDB(ZFProject* proj,
        const std::wstring& oldName, const std::wstring& newName);

    void renameScanInMeasurements(ZFProject* proj, 
        const std::wstring & oldName, const std::wstring & newName);

    void renameScanInAnnotations(ZFProject* proj, 
        const std::wstring & oldName, const std::wstring & newName);

    ZFSPRJ_API void updateChildNames(std::map<std::wstring, std::wstring> &scanMap);

    void setViewpointName(
        ZFProject* proj, 
        ZFProjectItemScan* zfs,
        const std::wstring & newViewposName);

//############################################################################

}
