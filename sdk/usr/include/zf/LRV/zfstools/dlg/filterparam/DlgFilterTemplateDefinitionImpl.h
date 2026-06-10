#pragma once
#include "../../ZfsToolsAPI.h"

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QDialog>
#include <QTreeWidgetItem>
SWITCH_WARNINGS_ON

#include <zfs/filter/filter_param.h>

class QTableWidgetItem;

namespace Ui {
    class DlgFilterTemplateDefinition;
}
class ZFProject;
namespace zfstools {

    /**
     * @brief filter templates modify
     */
    class ZFSTOOLS_API DlgFilterTemplateDefinitionImpl : public QDialog {
        Q_OBJECT

    public:

        DlgFilterTemplateDefinitionImpl(QWidget* parent, ZFProject* prj=nullptr, QList<QTreeWidgetItem*>* selScans=nullptr);
        ~DlgFilterTemplateDefinitionImpl();
        std::wstring getTemplateFile() { return m_filterTemplateFile; };
        bool changeOc{ false };

    private:
        Ui::DlgFilterTemplateDefinition* ui{};

        std::wstring m_filterTemplateFile;
        std::shared_ptr<zfs::FilterParams> m_params{};
        ZFProject& project;
        QList<QTreeWidgetItem*>* selecScans;

        /// search for all possible templates available
        void fillupTemplates();

        /// store the settings
        void store();

    private slots:
        void btnCancel();
        void btnImport();
        void btnExport();

        void templateChanged(int);
    };
} // end namespace zfstools
