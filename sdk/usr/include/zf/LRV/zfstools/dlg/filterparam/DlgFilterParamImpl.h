#pragma once
#include "../../ZfsToolsAPI.h"

#include <functional>
#include <memory>

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QDialog>
SWITCH_WARNINGS_ON

#include <zfs/filter/filter_param.h>

class QTableWidgetItem;

namespace Ui {
    class dlgfilterparam;
}

namespace zfstools {

/**
 * @brief filter configuration for a whole project
 */
    class ZFSTOOLS_API DlgFilterParamImpl : public QDialog {
        Q_OBJECT

    public:
        enum class EDIT_MODE {
            DEFAULT = 0,
            TEMPLATE,
            ACTIVATE_FILTERS,
            DEACTIVATE_FILTERS
        };
      /*
       * @brief Constructs a Dialog for setting up/loading/saving filter parameters
       * for the given project. The filters are loaded from the project file. Each
       * project can have a filter definition for a scanner type (e.g. 5010,
       * 9012...). The defaults are loaded from the associated files. The filenames
       * are determined based on the installation dir or the build root dir (for
       * development purposes).
       *
       * @param parent the parent widget of the dialog
       */
        DlgFilterParamImpl(QWidget* parent,
            std::shared_ptr<zfs::FilterParams> params,
            EDIT_MODE mode = EDIT_MODE::DEFAULT);

        ~DlgFilterParamImpl();


        typedef std::function< void(const std::shared_ptr<zfs::FilterParams>) > apply_cb;
        void apply_callback(apply_cb);

        void setDeactivateMode();

        void setActivateMode();

        void setTemplateMode();

        void enableApply();

        // callback called to setup the default values
        typedef std::function< void(std::shared_ptr<zfs::FilterParams>) > default_cb;
        void cbDefault(default_cb);

    private:
        Ui::dlgfilterparam* ui;
        std::string currentKey;
        apply_cb m_apply_f{};

    signals:
        void applyFilter(std::shared_ptr<zfs::FilterParams>);

    private slots:
        void apply();
        void btnOk();
        void btnCancel();
        void btnImport();
        void btnExport();
    };
} // end namespace zfstools
