#ifndef _ZFPRJ_LABELS_H_
#define _ZFPRJ_LABELS_H_

/**
    classes for labels
*/

#include <zfcore/filename.h>
#include "ZfsPrjBasic.h"
#include "scanattachedobject.h"

class ZFSHeader;
class ZFScan;

namespace tinyxml2 {
    class XMLDocument;
}

namespace zfprj {
    class ZFLabels;
    class ZFProjectPlan;
    //=============================================================================================

    /**
    \brief An project object that can be used to add text labels to scan points.

    For now the label always belongs to a scan point. To make arbitrary 3d-points
    possible, the LabelTool and the painters for the object need to be changed.
    */
    class ZFSPRJ_API ZFLabel : public ScanAttachedObject {
        DEFINE_ZFBASE(ZFPrjObj)

    public:
        static int mNextKenNr; // Kennummer fuer die naechste Kennumme eines Labels
        int mKenNr; // Kennummer eines Labels anhand derer gesucht werden kann, wird
                    // beim schliessen von LRC nicht gespeichert sondern beim Start
                    // neu vergeben.

    public:
        ZFLabel(ZFLabels *p, const std::wstring &shortScanName, QPoint scanPoint,
            const ZFVec3d &localPos, const ZFVec3d &globalPos);
        ~ZFLabel();

        virtual const char *Typ() { return "Label"; }
        virtual const char *Xml() { return "Label"; }

        std::wstring getLabelText() const { return mName.WString(); }
        void setLabelText(const std::wstring &t) { mName = t; }

        tinyxml2::XMLElement *createNodeFromLabel(tinyxml2::XMLDocument *doc);

    protected:
        virtual int getHeaderStartIndex();
        virtual int getHeaderMaxIndex();
        virtual std::wstring getHeaderStoredString();
    };

    //=============================================================================================

    /// Alle Links des Projectes
    class ZFSPRJ_API ZFLabels : public ZFPrjObjNode {
        DEFINE_ZFBASE(ZFPrjObjNode)

        // This index in the zfsheader contains the number of labels in the header.
        // At the following indexes we have the positions and text labels of the
        // labels
            static const int startIndexOfLabelsInHeader;
        static const int scannerLabelStartIndex;
        static const int maxNumberOfLabelsInHeader;

    public:
        typedef ZFLabel ElemType;

        ZFLabels(ZFProject *p) : ZFPrjObjNode(p) {}
        ~ZFLabels();

        virtual bool Load();
        virtual bool Store(double versNr);

        // Sucht alle Labels aus dem ZFS Header und fuegt sie dem Projekt hinzu.
        int CaptureFromZFS(ZFSHeader *);
        /**
        \brief Removes all links from the header;
        */
        static void removeAllObjectsFromHeader(ZFSHeader *header);
        QString Typ() const override { return QObject::tr("Labels"); }
        std::string Xml() const override { return "Labels"; } // xml sektion where to store

        /**
         * @brief Transfere old links to the new link structure (see ZFProjectPlan)
         *
         */
        void addLinkItem(zfprj::ZFProjectPlan *projectPlan,
            const std::wstring &shortScanName, const ZFVec3d &localPos,
            const std::wstring labelText = L"");

        /**
         * @deprecated use addLinkItem method to add links!
         */
        ZFLabel *AddLabel(const std::wstring &text, const std::wstring &shortScanName,
            QPoint scanPoint, const ZFVec3d &localPos,
            const ZFVec3d &globalPos);

        /**
         * @deprecated use addLinkItem method to add links!
         */
        ZFLabel *AddLabel(ZFLabel *label);

        /*ZFLabel* FindObjectByText( const std::wstring& text, const ZFVec3d& localPos
        ) const; // link nach text suchen ZFLabel* FindLabelByPos( const ZFVec3d&
        localPos ); ZFLabel* findLabelByKenNr( int nr ); ZFLabel* FindLabelByUVPos(
        int u, int v, const std::wstring& scan ) const; bool DelLabel( const ZFVec3d&
        localPos ); bool DelLabel( int nr );*/

        /**
        \brief At this index the ZFSHeader contains the number of labels that are
        stored in the header. At the following indexes it contains the labels.
        */
        static int getHeaderStartIndexOfLabels();

        /**
        \brief The first index of the ZFSHeader that does not contain labels any more.
        */
        static int getHeaderMaxIndexOfLabels();
    };

    //=============================================================================================

} // end namespace zfprj

#endif //_ZFPRJ_LABELS_H_
