#pragma once

#include "QToolsApi.h"

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
SWITCH_WARNINGS_ON

#include "metadatafactory.hpp"
#include "../../zfsprj/metadata.h"

namespace zfqtools {

    class MetadataWidgetFactory;

    //#####################################################################################

    class QTOOLS_API MetadataWidget : public QTableWidget {
        Q_OBJECT

        zfprj::ZFMetadataList metadataList;
        MetadataWidgetFactory* factory;

    public:

        MetadataWidget(QWidget* parent);
        
        /**
         * @brief Create widgets for the metadata list. The given factory is used to create the widgets.
         *
         * @param metadataList  the list to be displayed
         * @param factory       the widget factory used to create the display widgets
         *
         * @return  true, if the wigdets have been created successfully
         */
        bool createMetadataList(zfprj::ZFMetadataList metadataList, MetadataWidgetFactory* factory);


        MetadataWidgetFactory* getFactory() {
            if (factory == nullptr) {
                factory = new DisplayMetadataWidgetFactory();
            }
            return factory;
        }
    };

}


