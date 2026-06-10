#ifndef DXFFILEWRITER_H
#define DXFFILEWRITER_H

#include "zftools_api.h"
#include <stdio.h>

#include "fileexportdatatypes.h"

#include <zfs/trmatrix.h>
namespace zftools {

//###################################### Class DxfFileWriter ###############################
/**
\brief This class can be used to store data into dxf files.

Usage:
A DXF file will be created when constructing a DxfFileWriter object.
You can then give the dxfFileWriter the data that you want to write to the file with the addMyData() functions.
When the dxfFileWriter object is destroyed, it will write the collected data to the file and close it.

Internal information:
When you want to add new kinds of data that can be written to a file you must add it to the Layer class.
Also add a public addMyData function that stores the information in the data member, and then edit the
flushDataToFile() function where you have to specifiy how the data will be written to the file.
*/
    class ZFTOOLS_API DxfFileWriter {
    public:

    private:
        DxfFileStringDefines stringDefines; ///< This object holds the strings that are used to compose a dxf file

        FILE* openedFile;
        bool ok;

        // members that hold the data
        std::vector<LayerDxfFileWriter> layers;  ///< The Layer Objects hold the data that is written to the file

        int defaultColor;
        std::vector<char> disallowedCharacters;

        ZFTRMatrix worldMatrix;

    public:
        DxfFileWriter(const std::wstring& fileName, bool& isOk);
        ~DxfFileWriter();

        bool isOk() { return ok; }  ///< Is true when the file could be opened
        void addLayer(std::string layername, int colorIndex);  ///< Edit this function when you add new data members.
        void addPointToLayer(const ZFVec3dDxf& point, const char* layerName);
        void addTextLabelToLayer(ZFVec3dDxf textlabel, const char* layerName);
        void addPointWithTextLabelToLayer(ZFVec3dDxf point, const char* layerName);
        void addPointsWithTextLabelsToLayer(std::vector<ZFVec3dDxf> points, const char* layerName);
        void addPointsToLayer(const std::vector<ZFVec3dDxf>& points, const char* layerName);
        void addTextLabelsToLayer(const std::vector<ZFVec3dDxf> points, const char* layerName);
        void addPolygonalLineToLayer(const std::vector<ZFVec3dDxf>& polygonalLine, const char* layerName);
        void addPointToPolygonalLine(const ZFVec3dDxf& point, const char* layerName);
        void addFaceToLayer(DxfFace face, const char* layerName);
        void addFacesToLayer(std::vector<DxfFace> faces, const char* layerName);

        void setWorldMatrix(const ZFTRMatrix& _worldMatrix) {
            worldMatrix = _worldMatrix;
        }

    private:
        void init();
        void flushDataToFile();

        void addPointsToFile(LayerDxfFileWriter &layer);
        void addPolylinesToFile(LayerDxfFileWriter &layer);
        void addTextLabelsToFile(LayerDxfFileWriter &layer);
        void addFacesToFile(LayerDxfFileWriter &layer);

        size_t getLayerIndexForNewOrExistingLayer(const char* layerName);

        std::string toDxfAllowedName(std::string string);

        const ZFVec3dDxf getWorldPoint(const ZFVec3dDxf& point) const;
        const DxfFace getWorldFace(const DxfFace& face) const;
    };


}; // end namespace zftools

#endif // DXFFILEWRITER_H
