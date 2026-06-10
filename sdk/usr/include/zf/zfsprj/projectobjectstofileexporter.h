#ifndef PROJECTOBJECTSTOFILEEXPORTER
#define PROJECTOBJECTSTOFILEEXPORTER

#include <string>

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QObject>
SWITCH_WARNINGS_ON

#include "ZfsPrjBasic.h"
#include "Project.h"

#include "../zftools/fileexportdatatypes.h"

namespace zfprj
{

class ZFPrjObjNode;
class ZFPrjObj;
class Layers;
class MessLayer;
class ZFPolygon;
class ZFPlane;
class ZFShootChannel;
class ZFPlanePolyPnt;

/**
\brief This class exports the project object tree to various file formats.
*/
class ZFSPRJ_API ProjectObjectsToFileExporter
{
public:

private:
    zfprj::Layers* layers;

public:
    ProjectObjectsToFileExporter();

    void setLayersForExport( zfprj::Layers* layersArg) {
        layers = layersArg;
    }

    ///< This function creates a file with filename in the given format format that contains all project objects that have set ZFPrjObj::mExport to true.
    bool exportObjectTreeToFileFormat(
        const std::wstring& filenameWithoutExtension,
        const ZFTRMatrix& worldMatrix,
        ZFProject::ExportFormat format); 

    ///< Returns true if the objects in node can be exported to a file with format.
    static bool exportOfObjectInFormatIsSupported(
        ZFPrjObjNode* node, ZFProject::ExportFormat format); 

    ///< Returns true if the object can be exported to a file with format.
    static bool exportOfObjectInFormatIsSupported(
        ZFPrjObj* object, ZFProject::ExportFormat format);

    std::wstring getExportExtension(
        ZFProject::ExportFormat format );

private:
    bool exportLayersInDxfFormat(        
        const std::wstring& filename,
        const ZFTRMatrix& worldMatrix);

};


}	// end namespace


#endif // !PROJECTOBJECTSTOFILEEXPORTER
