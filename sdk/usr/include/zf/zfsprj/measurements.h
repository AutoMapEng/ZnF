#pragma once

// Attributnamen fuer XML-Export einer Messung
#define PRJXML_SRC		"src"
#define PRJXML_Q		"q"
#define PRJXML_END		"end"

/**
    classes for measurements as layers
*/
#include "ZfsPrjBasic.h"
#include "obj.h"

#include <zfcore/filename.h>
#include <zfs/file_extensions.h>
#include <zfs/zfvector.h>

#include <string>
#include <ostream>
#include <vector>

class ZFScan;

namespace zfprj
{
    class MessLayer;
    class Messungen;

    /**
    \brief One point in a MessLayer poly line.
    */
    class ZFSPRJ_API MessPoint
    {
        friend class Messungen;

    public:
        std::wstring scan;	// scan ohne extension zu dem der messpunkt gehoert, messlienie kann ueber mehrere scans gehen
        int u, v;        // bildcoordinaten
        bool endpoint;  // true if this is the last point in a chain
        double x, y, z;   // messpunkt
        enum { SRC_UNKNOWN, SRC_TARGET, SRC_PT };
        int source;		// art und weise wie der punkt erzeugt wurde
        enum { Q_UNKNOWN = 0, Q_TARGET = 100, Q_PT = 10 }; // percent
        float quality;	// 100 = vom targetfinder = beste qualitaet

    public:
        MessPoint();
        MessPoint(const std::wstring& sc, int u_, int v_, double _x, double _y, double _z, int src = SRC_UNKNOWN, double q = Q_UNKNOWN, bool ende = false);

        virtual ~MessPoint() {}

        //virtual const char* Typ() const { return "Measurementpoint"; }
        virtual const char* Xml() const { return "Point"; }

        const wchar_t* ScanName() const { return scan.c_str(); }

        std::wstring scanNameWithExtention() {
            zf::FileName filename(scan);
            if (!filename.endsWith(ext_zfs)) {
                filename.AddExt(ext_zfs);
            }
            return filename.WString();
        }

        void SetAsEndPoint(bool b) { endpoint = b; }

        void set3D(const ZFVec3d& vec);
        ZFVec3d get3D() const;

        static double distance3D(MessPoint p1, MessPoint p2);
    };

    ZFSPRJ_API bool operator==(const MessPoint& lhs, const MessPoint& rhs);
    ZFSPRJ_API std::ostream& operator<<(std::ostream& stream, const MessPoint& rhs);

    //------------------------------------------------------------------------------------------------------
    typedef std::vector<MessPoint> MessPoints;

    std::vector<ZFVec3d> ZFSPRJ_API messPointsToZFVecs(const MessPoints& messPoints);

    /**
    \brief A MessLayer contains multiple points that form a polygonal line that is used for measuring distances between those points.
    */
    class ZFSPRJ_API MessLayer : public ZFPrjObj
    {
        DEFINE_ZFBASE(ZFPrjObj)
            friend class Messungen;

    public:
        bool visible;	// signal to draw or not draw.
        bool underConstruction;	// signal that this one is currently beeing edited.

    protected:
        std::vector<MessPoint> messPoints;

    public:
        MessLayer(const std::wstring& name);
        MessLayer(const std::wstring& layer, ZFPrjObjNode* parent, const std::wstring& name = L"", int col = -1);
        virtual ~MessLayer();

        void tansformPointCoordinates(ZFTRMatrix oldToNewSystemMatrix);    ///< Applies the transformation matrix to all points in messPoints.

        virtual bool Load() { return false; }
        virtual bool Store(double versNr) { return false; }
        //const char* Typ() const override { return "Points"; }
        std::string Xml() const override { return "Points"; }  // xml sektion where to store
        virtual const QPixmap* GetPixmap();

        //void InsertPoint(MessPoint&);
        //void Undo();
        MessPoint AddPt(const std::wstring& zfs, int u, int v, double x, double y, double z, int src, double q, bool endpoint = false);
        void AddPt(const MessPoint& point);
        void AddEndPt( const MessPoint& point );
        MessPoint GetLastPt() const;
        MessPoint GetPointBeforeLastPoint() const;
        bool RemoveLastPt(); // undo
        void clearMessPoints();
        size_t size() const { return messPoints.size(); }

        void renameScan( const std::wstring newName, const std::wstring oldName )
        {
            for( auto& point : messPoints )
            {
                if( point.scan == oldName )
                {
                    point.scan = newName;
                }
            }
        }

        void replacePoint( int nr, const MessPoint pt )
        {
            messPoints[nr] = pt;
        }

        const std::vector<MessPoint>& getMessPoints() const { return messPoints; }

        bool IsVisible() const { return visible; }
        void SetVisible(bool vis = true) { visible = vis; }

        double getTotalLength() const;
        std::vector<MessPoints> getPiecesWithOnlyOneScan() const;    /**< This cuts messPoints into pieces at the lines where the scan changes from one point to the next. The returned pieces only contain points that belong to the same scan. */
    };

    //------------------------------------------------------------------------------------------------------

    /// A collection of MessLayer objects.
    class ZFSPRJ_API Messungen : public ZFPrjObjNode
    {
        DEFINE_ZFBASE(ZFPrjObjNode)

    private:
        static int nameCounter;                 ///< This is used for generating the default name of a plane.
        static std::wstring defaultNamePrefix;  ///< This is used for generating the default name of a plane.

    public:
        Messungen(ZFProject*);

        virtual bool Load();
        virtual bool Store(double versNr);
        QString Typ() const override { return QObject::tr("Measurement Layers"); }
        std::string Xml() const override { return "Measurements"; }  // xml sektion where to store

        virtual void Free() { ZFPrjObjNode::Free(); }

        void Activate(const std::wstring&);
        //MessLayer* Current() { return current; } // activer layer
        MessLayer* AddLayer(const std::wstring& name = L"");    ///< Ownership stays with Messungen
        MessLayer* AddLayer(MessLayer* messlayer);             ///< returns the argument, Ownership goes to Messungen
        void DelLayer(const std::wstring& name);
        MessLayer* FindLayer(const std::wstring& name);
        MessLayer* getLastLayer();

        void Cleanup(); // alles loeschen und dummylayer anlegen
        void SetAllVisible(bool vis = true);
        void setAllInactive();

        static void setDefaultNamePrefix(std::wstring prefix) { defaultNamePrefix = prefix; }
        static std::wstring getDefaultName();
    };
} // end namespace zfprj
