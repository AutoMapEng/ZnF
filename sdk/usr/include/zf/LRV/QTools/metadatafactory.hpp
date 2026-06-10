#pragma once

#include <memory>

#include "QToolsApi.h"

#ifndef LINUX
#include "MyApp.h"
#endif

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QMouseEvent>
#include <QGridLayout>
SWITCH_WARNINGS_ON

#include "../../Common/zfstring.h"
#include <zfcore/tinyxml2.h>
#include "../../zfsprj/metadata.h"

namespace zfqtools {
    //#####################################################################################

    /**
    * @brief Widget for editing textual metadata.
    */
    class QTOOLS_API EditMetadataWidget : public QLineEdit
    {
        Q_OBJECT

        std::shared_ptr<zfprj::ZFMetadata> metadata;

    public:
        EditMetadataWidget(std::shared_ptr<zfprj::ZFMetadata> _metadata, QWidget* parent) : QLineEdit(_metadata->value.toString(), parent), metadata(_metadata) {
            connect(this, SIGNAL(textEdited(const QString &)), this, SLOT(setValue(const QString &)));
            connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(setValue(const QString &)));
        }

    public slots:
        void setValue(const QString& newValue) {
            metadata->value = newValue;
        }

        std::shared_ptr<zfprj::ZFMetadata> getMetadata() {
            return metadata;
        }

    };

    //#####################################################################################

    /**
    * @brief Factory to retrieve taxonomy lists
    */
    class QTOOLS_API TaxonomyFactory
    {
        tinyxml2::XMLDocument taxonomyDoc;

        bool init;
    private:

        TaxonomyFactory() : init(false) {
        }

        ~TaxonomyFactory() {}

    public:
        bool isInit();

        /**
        * @brief Access-Method to the TaxonomyFactory instance.
        * @return the instance
        */
        static TaxonomyFactory *getInstance();

        /**
         * @brief Read in the taxonomy file.
         *
         * @return true, if the taxinomy was loaded successfully
         */
        bool readTaxonomy(const QString& configPath);

        /**
         * @brief Get a list of strings for the given taxonomy path.
         *
         * The taxonomy has following structure:
         *
         * <pre>
         * <Taxonomy name="Basic">
         *       <name>
         *           <subname type="QString">
         *               <entry name="1" comment="first entry"/>
         *               <entry name="2" comment="second entry"/>
         *               <entry name="3" comment="third entry"/>
         *           </subname>
         *       </name>
         *   </Taxonomy>
         * </pre>
         *
         * To get the list for name/subname call getTaxonomyList("name/subname");
         *
         * @param path  the path to the list
         *
         * @return the list of taxonomy entries for the path. If the path was not found, this list is empty
         */
        QStringList getTaxonomyList(const QString& path);
    };

    //#####################################################################################

    /**
    * @brief Widget for editing textual metadata with taxonomy selection.
    */
    class QTOOLS_API EditTaxonomyMetadataWidget : public QComboBox
    {
        Q_OBJECT

        std::shared_ptr<zfprj::ZFMetadata> metadata;

    public:
        QStringList taxonomyStings;

        EditTaxonomyMetadataWidget(std::shared_ptr<zfprj::ZFMetadata> _metadata, QWidget* parent) : QComboBox(parent), metadata(_metadata) {
            setObjectName("EditTaxonomyMetadataWidget");

            taxonomyStings = TaxonomyFactory::getInstance()->getTaxonomyList(metadata->taxonomyPath);

            addItems(taxonomyStings);
            setEditable(true);
            setCurrentText(metadata->value.toString());
            connect(this, SIGNAL(currentTextChanged(const QString &)), this, SLOT(setValue(const QString &)));
        }

    public slots :
        void setValue(const QString& newValue) {
            metadata->value = newValue;
        }

        std::shared_ptr<zfprj::ZFMetadata> getMetadata() {
            return metadata;
        }

    protected:
        void mousePressEvent(QMouseEvent * event) {
            if(event->button() == Qt::LeftButton)
            {
#ifdef LINUX
                // workaround for blocking popups
                hidePopup();
                return;
#endif
            }
            QComboBox::mousePressEvent(event);
        }
    };

    //#####################################################################################

    /**
    * @brief Factory to generate QWidget instances for Metadata objects.
    */
    class QTOOLS_API MetadataWidgetFactory : public QObject {
        Q_OBJECT
    public:

        MetadataWidgetFactory() : QObject() {}

        virtual QWidget* createWidget(std::shared_ptr<zfprj::ZFMetadata> metadata, QWidget* parent) {
            return new QWidget(parent);
        }
    };

    //#####################################################################################
    
    /**
    * @brief Metadata-Factory to generate QWidget instances for Metadata objects.
    *
    * The generated widgets allow editing of the displayed values.
    */
    class QTOOLS_API EditMetadataWidgetFactory : public MetadataWidgetFactory
    {
        Q_OBJECT

    public:
        EditMetadataWidgetFactory(const QString& path = "") {
            if (!TaxonomyFactory::getInstance()->isInit())
            {
                TaxonomyFactory::getInstance()->readTaxonomy(path);
            }
        }

        virtual QWidget* createWidget(std::shared_ptr<zfprj::ZFMetadata> metadata, QWidget* parent) {
            QWidget* editField;
            if (metadata->taxonomyPath.isEmpty()) {
                editField = new EditMetadataWidget(metadata, parent);
            } else {
                editField = new EditTaxonomyMetadataWidget(metadata, parent);
            }

            return editField;
        }

    };

    //#####################################################################################

    /**
    * @brief Metadata-Factory to generate QWidget instances for Metadata objects.
    *
    * The generated widgets only support display of metadata.
    */
    class QTOOLS_API DisplayMetadataWidgetFactory : public MetadataWidgetFactory
    {
        Q_OBJECT

    public:
        DisplayMetadataWidgetFactory() {}

        virtual QWidget* createWidget(std::shared_ptr<zfprj::ZFMetadata> metadata, QWidget* parent) {
            QLineEdit* lineEdit = new QLineEdit(metadata->value.toString(), parent);
            lineEdit->setEnabled(false);
            return lineEdit;
        }

    };

}
