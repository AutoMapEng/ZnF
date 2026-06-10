#ifndef SCANATTACHEDOBJECT_H
#define SCANATTACHEDOBJECT_H

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QPoint>
SWITCH_WARNINGS_ON

#include "../zfs/zfvector.h"

#include "ZfsPrjBasic.h"
#include "obj.h"

class ZFScan;
class ZFSHeader;

namespace zfprj
{

    /**
    \brief Base class for ZFLink and ZFLabel

    The class is for objects that belong to a ZFScan and have a 2d-postion in the scan.

    All objects of one sub-type of ScanAttachedObject must have unique positions in their scan.
    This is necessary because the positions are used as identifier when objects are synced with the
    ZFSHeader.
    */
    class ZFSPRJ_API ScanAttachedObject : public ZFPrjObj
    {
        DEFINE_ZFBASE(ZFPrjObj)

        zf::FileName scanName;              // scanname zu dem der link gehoert
        QPoint flatViewPoint;               // position im scan
        ZFVec3d localPosition;              ///< XYZ Position, local scan coordinates, this only needs to be set when the object is created
        ZFVec3d globalPosition;             ///< The global position must be updated when the position of the scan changes. 
                                            // This is done externally when the registration changes or the project is opened.
                                            // The global position is needed for drawing the label in the 3d-view.

    public:
        ScanAttachedObject(ZFPrjObjNode* parent, const std::wstring& shortScanName , QPoint scanPoint, const ZFVec3d& localPos, const ZFVec3d& globalPos);

        std::string Xml() const override { return "ScanAttachedObejct"; }

        void setScanName( const zf::FileName& name);
        std::wstring ScanFileNameW() const; // short filename
        std::string ScanFileName() const;

        void setFlatViewPoint( const QPoint& point);
        QPoint getFlatViewPoint() const;

        ZFVec3d getLocalPosition() const;
        void setLocalPostion( const ZFVec3d& pos);

        ZFVec3d getGlobalPosition() const;
        void setGlobalPosition( const ZFVec3d& pos);

        /**
        If a label at the this labels position already exists in the header it will be overwritten.
        Otherwise a new label will be added to the header.
        */
        bool writeObjectToScanHeader(ZFSHeader* header);
        bool deleteObjectFromScanHeader(ZFSHeader* header);

        /**
        Stores the x and y coordinate of point in one int. This format is used to 
        store the position in the ZFSHeader.
        */
        static int getPosInt(const QPoint& point);
        static QPoint getPosFromInt( unsigned int i);

    protected:
        virtual int getHeaderStartIndex() = 0;
        virtual int getHeaderMaxIndex() = 0;
        virtual std::wstring getHeaderStoredString() = 0;

    private:
        int getIndexOfObjectInScanHeader(ZFSHeader* header);  ///< returns -1 if the label is not in the header
        int getFirstFreeObjectIndexInScanHeader(ZFSHeader* header);  ///< returns -1 if no slots are free

        void writeObjectToHeader(ZFSHeader* header, int index, bool overwrite);
        void deleteObjectFromHeader(ZFSHeader* header, int index);

    };

}

#endif