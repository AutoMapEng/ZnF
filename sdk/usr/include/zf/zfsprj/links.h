#ifndef _ZFPRJ_LINKS_H_
#define _ZFPRJ_LINKS_H_

/**
    classes for linkmanagement
*/

#include "ZfsPrjBasic.h"
#include "scanattachedobject.h"
#include <zfcore/filename.h>

#include <string>

class ZFSHeader;
class ZFScan;

namespace zfprj {
    class ZFLinks;
    class ZFProjectPlan;

    //=============================================================================================
    /**
    * \brief Links a scan point to a filename.
    *
    * This can be used to add external references to a point in the scan.
    *
    * @deprecated
    */
    class ZFSPRJ_API ZFLink : public ScanAttachedObject {
        DEFINE_ZFBASE(ZFPrjObj)

    public:
        static const std::wstring linkedFileFolderName;

    private:
        zf::FileName linkedFile;    // Datei die verlinkt wurde. Kann relativ oder absolut sein.
        std::wstring labelText;         // Der text des links der in der view zu sehen ist.

    public:
        ZFLink(ZFLinks* p, const std::wstring& shortScanName, QPoint scanPoint, const ZFVec3d& localPos, const ZFVec3d& globalPos);

        std::string Xml() const override { return "Link"; }

        std::wstring getLinkedFile() const;
        void setLinkedFile(const std::wstring& filename);  ///< Filename can be relative to the project or scan, or absolute.

        std::wstring getLabelText() const;
        void setLabelText(const std::wstring&);

    protected:
        virtual int getHeaderStartIndex();
        virtual int getHeaderMaxIndex();
        virtual std::wstring getHeaderStoredString();
    };

    //=============================================================================================

    /// A collection of all ZFLink objects in the project.
    // @deprecated
    class ZFSPRJ_API ZFLinks : public ZFPrjObjNode {
        DEFINE_ZFBASE(ZFPrjObjNode)

            static const int maxNumberOfLinksInHeader;
        static const int startIndexOfLinksInHeader;

    protected:
        std::wstring folder; // folder where linkfiles will be stored

    public:
        typedef ZFLink ElemType;

        ZFLinks(ZFProject*);

        /**
        * @brief Loads all Links and stores them into the new structure (see ZFProjectPlan). The links are not added to the ZFLinks list
        *        of the project. This is only done to transfere old links. ZFLinks is in general deprecated.
        */
        virtual bool Load();

        /**
        * @deprecated This function is still called, but it should always receive an empty link list.
        *             The old links are getting transfered into the new project planning structure at load time
        */
        virtual bool Store(double versNr);

        /**
        * @brief Sucht alle links aus dem ZFS Header und fuegt sie dem Projekt hinzu. Diese enthalten keinen Text
        */
        int CaptureFromZFS(ZFSHeader*);

        /**
        \brief Removes all links from the header;
        */
        static void removeAllObjectsFromHeader(ZFSHeader* header);

        QString Typ() const override { return QObject::tr("Links"); }
        std::string Xml() const override { return "Links"; }  // xml sektion where to store

        /**
        * @brief Transfere old links to the new link structure (see ZFProjectPlan)
        *
        */
        void addLinkItem(zfprj::ZFProjectPlan* projectPlan, const std::wstring& linkedFile, const std::wstring& shortScanName, const ZFVec3d& localPos, const std::wstring labelText = L"");

        /**
        * @deprecated use addLinkItem method to add links!
        */
        ZFLink* AddLink(const std::wstring& linkedFile, const std::wstring& shortScanName, QPoint scanPoint, const ZFVec3d& localPos, const ZFVec3d& globalPos, const wchar_t* labelText = NULL);

        /**
        * @deprecated use addLinkItem method to add links!
        */
        ZFLink* AddLink(ZFLink* link);

        /*ZFLink* FindObjectByText( const std::wstring& text, const ZFVec3d& localPos );
        ZFLink* FindLinkByText( const std::wstring& text);  // link nach text suchen
        ZFLink* FindLinkByFileName( const std::wstring& filename, const std::wstring& scan ); // link nach text suchen
        ZFLink* FindLinkByUVPos( int u, int v, const std::wstring& scan ) const;*/

        /**
        Finds links within a ten point radius of the given scan point.
        */
        // ZFLink* FindCloseLinkByUVPos( const QPoint& scanpoint, const std::wstring& shortScanName) const;
        // bool DelLink( const std::wstring& text, const std::wstring& file );
        // bool DelLink( int u, int v, const std::wstring& scan );

        const wchar_t* Folder() { return folder.c_str(); }

        /**
        \brief At this index the ZFSHeader contains the number of links that are stored in the header.
        At the following indexes it contains the labels. Note that only the linked file is stored in
        the header, but not the label text.
        */
        static int getHeaderStartIndexOfLinks();

        /**
        \brief The first index of the ZFSHeader that does not contain labels any more.
        */
        static int getHeaderMaxIndexOfLinks();
    };

    //=============================================================================================


} // end namespace zfprj

#endif //_ZFPRJ_LINKS_H_
