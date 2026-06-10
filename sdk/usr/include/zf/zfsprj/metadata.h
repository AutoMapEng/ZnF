#pragma once

#include <memory>
#include <vector>

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QVariant>
#include <QString>
SWITCH_WARNINGS_ON

#include "ZfsPrjBasic.h"
#include "obj.h"
#include "xml.h"

#include "../zfs/zfsheader.h"

#define LINK_METADATA_TEMPLATE
#define PROJECT_METADATA_TEMPLATE

namespace tinyxml2 {
    class XMLNode;
    class XMLElement;
    class XMLDocument;
}

namespace zfprj {
    class ZFMetadataList;

    //=============================================================================================

    /**
    * @brief Metadata entry
    */
    class ZFSPRJ_API ZFMetadata {

    public:
        QString key;
        QVariant value;

        QString taxonomyPath;
        QString type;

        QString comment;

        ZFMetadata(const QString& key, const QVariant& value, 
            const QString& taxonomyPath = "", 
            const QString& type = "QString", const QString& comment = "");
        ~ZFMetadata();

    protected:
    };

    //=============================================================================================

    /**
    * @brief List of metadata entry
    */
    class ZFSPRJ_API ZFMetadataList {

        std::list<std::shared_ptr<zfprj::ZFMetadata>> metadataList;

    public:
        ZFMetadataList() {};
        ~ZFMetadataList();

        /**
         * @brief Add a metadata entry to the list.
         *
         * @param key           the key of the entry
         * @param value         the current value of the entry
         * @param taxonomyPath  the path in the taxonomy tree, if applicable
         *
         * @return  the pointer to the added metadata object
         */
        std::shared_ptr<zfprj::ZFMetadata> addMetadata(const QString& key,
            const QVariant& value, const QString& taxonomyPath = "", 
            const QString& type = "QString", const QString& comment = "");

        /**
         * @brief Create the metadata list from the given node.
         *
         * @param metadataNode  the node to be used to create the metadata from
         * @param templateName  the template name for the metadata template
         *
         * @return true, if the metadata list was created successfully
         */
        bool createFromNode(tinyxml2::XMLNode* metadataNode,
            const QString& templateName = "");

        /**
         * @brief Create metadata list from the given template file. The file must be located in the /Data/Metadata folder of the
         * Application.
         *
         * @param templateName name of the template file.
         *
         * @return true, if the metadata list was created
         */
        bool createFromTemplate(const QString & templateName, 
            const QString & path = "");

        /**
         * @brief Create metadata list from the given scan header.
         *
         * @param header scan header
         *
         * @return true, if the metadata list was created
         */
        bool createFromHeader(const std::shared_ptr<ZFSHeader> header);
        bool createFromHeader(const ZFSHeader* header);

        /**
         * @brief Write the metadata list to the given node.
         *
         * @param parentNode    the node, where the metadata list should be added
         *
         * @return  true, if the metadata list was stored in the node
         */
        bool writeToNode(tinyxml2::XMLNode* parentNode);

        static tinyxml2::XMLElement* createXMLNode(
            tinyxml2::XMLDocument* doc,
            const QString& key, const QVariant& value = "",
            const QString& taxonomy = "", const QString& comment = "");

        QString getStringForMetadata(const QString& formatString = "%1_@_%2_@_%3_|_");

        // check if there is some value is defined in the list
        bool isAnyDefined() const;

        /**
         * @brief Get access to the underlaying metadata list.
         *
         * @return the list of shared pointers.
         */
        std::list<std::shared_ptr<zfprj::ZFMetadata>>& getList() { return metadataList; }
        const std::list<std::shared_ptr<zfprj::ZFMetadata>>& getList() const { return metadataList; }

        void createFromString(QString metadataString);
    };

    //=============================================================================================

}
