#pragma once
#include <memory>

/**
\file objectexportdelegator.h
Maybe the functionality in this file should be moved to the export target.
*/

#pragma warning(push)
#pragma warning(disable: 4610 4512 4510)
#pragma warning(pop)

#include "../zftools/fileexportdatatypes.h"

#include "Project.h"

namespace zftools
{
    class DxfFileWriter;
}

namespace zfprj
{

class ZFPrjObj;
class ProjectObjectExporter;
class MessLayer;
class ZFPolygon;
class ZFPlane;
class ZFShootChannel;
class ZFPlanePolyPnt;

//###################################################################################
/**
\brief Delegates the export of an object in a certain format to a subclass of the ProjectObjectExporter.
*/
class ObjectExportDelegator
{
private:
    std::shared_ptr<ProjectObjectExporter> subclassObject;

public:
    ObjectExportDelegator( ZFPrjObj* object);

    bool objectSupportsExportInFormat( ZFProject::ExportFormat format);
    void addObjectToDxfFileWriter( zftools::DxfFileWriter* fileWriter);
};

//###################################################################################
/**
\brief Common interface for exporting various kinds of ZFPrjObj to various file formats.
*/
class ProjectObjectExporter
{
public:
    virtual bool supportsExportInFormat( ZFProject::ExportFormat format) = 0;
    virtual void addToDxfFileWriter( zftools::DxfFileWriter* fileWriter) = 0;
};

//###################################################################################
/**
\brief this class is used whenever the there was no ProjectObjectExporter subclass implemented for a certain PrjObj
*/
class NotSupportedObjectExporter : public ProjectObjectExporter
{
public:
    NotSupportedObjectExporter( ZFPrjObj* object){;}

    virtual bool supportsExportInFormat( ZFProject::ExportFormat format){ return false;}
    virtual void addToDxfFileWriter( zftools::DxfFileWriter* fileWriter){;}
};

//###################################################################################
/**
\brief A ProjectObjectExporter for MessLayer objects.
*/
class MessLayerExporter : public ProjectObjectExporter
{
private:
    MessLayer* messlayer;

public:
    MessLayerExporter( ZFPrjObj* object);

    virtual bool supportsExportInFormat( ZFProject::ExportFormat format);
    virtual void addToDxfFileWriter( zftools::DxfFileWriter* fileWriter);
};

//###################################################################################
/**
\brief A ProjectObjectExporter for ZFPolygon objects.
*/
class ZFPolygonExporter : public ProjectObjectExporter
{
private:
    ZFPolygon* polygon;

public:
    ZFPolygonExporter( ZFPrjObj* object);

    virtual bool supportsExportInFormat( ZFProject::ExportFormat format);
    virtual void addToDxfFileWriter( zftools::DxfFileWriter* fileWriter);
};

//###################################################################################
/**
\brief A ProjectObjectExporter 
*/
class ZFPlaneExporter : public ProjectObjectExporter
{
private:
    ZFPlane* plane;

public:
    ZFPlaneExporter( ZFPrjObj* object);

    virtual bool supportsExportInFormat( ZFProject::ExportFormat format);
    virtual void addToDxfFileWriter( zftools::DxfFileWriter* fileWriter);

private:
    zftools::ZFVec3dDxf planePointToZFVec3dDxf( const ZFPlanePolyPnt& planePoint);
};

//###################################################################################
/**
\brief A ProjectObjectExporter for ZFShootChannel objects.
*/
class ZFShootChannelExporter : public ProjectObjectExporter
{
private:
    ZFShootChannel* shootChannel;

public:
    ZFShootChannelExporter( ZFPrjObj* object);

    virtual bool supportsExportInFormat( ZFProject::ExportFormat format);
    virtual void addToDxfFileWriter( zftools::DxfFileWriter* fileWriter);
};

}   // end namespace

