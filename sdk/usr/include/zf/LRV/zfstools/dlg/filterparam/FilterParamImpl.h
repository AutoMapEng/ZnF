#pragma once
#include "../../ZfsToolsAPI.h"

#include <functional>
#include <memory>

#include <QObject>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QLineEdit>
#include <QCheckBox>
#include <QColor>
#include <QPushButton>

#include <Common/def.h>

#include <zfs/filter/filter_param.h>

namespace Ui {
    class filterparameter;
}

//#########################################################################

class ColorChooserButton : public QPushButton {
    Q_OBJECT

public:
    explicit ColorChooserButton(QWidget* parent = nullptr);

    QColor getColor() const;
    void setColor(QColor col);

signals:
    void colorChanged();

private slots:
    void openColorDialog();

private:
    QColor selectedColor;
};

//#########################################################################

namespace zfstools {
    /**
     * @brief Widget to display a filter definition in a list view.
     */
    class FilterParamLabel : public QTableWidgetItem {
        zfs::FilterParamItem *filterParameter;

    public:
        explicit FilterParamLabel(zfs::FilterParamItem *parameter);

        zfs::FilterParamItem *getFilterOptions();

        void init();
    };

    //#########################################################################

    /**
     * @brief Widget to edit filter parameter
     */
    class ZFSTOOLS_API FilterParamImpl : public QWidget {
        Q_OBJECT

    public:
        explicit FilterParamImpl(QWidget *parent);
        FilterParamImpl(QWidget *parent, std::shared_ptr<zfs::FilterParams>);

        ~FilterParamImpl() override;

        // set begin with showing the advanced filter
        void setAdvanced() { m_advanced = true; }

        std::shared_ptr<zfs::FilterParams> getParams() const;
        void setParams(std::shared_ptr<zfs::FilterParams> _params, bool showInfo=true );
        void resetDefaults();
        void allOff(bool intensityAsWell=true);

        bool reloadFromFile(const QString &fn);
        bool storeToFile(const QString &fn) const;

        void updateFilterColor() const;

        void hideParameter() const;

        // callback called to setup the default values
        typedef std::function< void(std::shared_ptr<zfs::FilterParams>) > default_cb;
        void cbDefault(default_cb cb) { cb_default = cb; }

        int getCurrentSelected() const;
        void setCurrentSelected(int idx);

    private:
        Ui::filterparameter *ui;
        std::shared_ptr<zfs::FilterParams> params;
        bool m_advanced{};
        default_cb cb_default{};

        std::string currentFilter;
        std::string currentAttribute;

        void fill();
        void printInfo();

    signals:
        void applyFilter(std::shared_ptr<zfs::FilterParams>);

    public slots:
        void parameterClicked(const std::string& parameterName);
        void filterClicked(const std::string& filterName);
        void handleItemChanged(QTableWidgetItem *item);

        void init();
    };

    //#########################################################################

    /**
    * @brief Validator to be used in the filter options table.-
             Sets different value checks based on the given FilterParamItem.
    */
    class ValidatedFilterItem : public QItemDelegate {
        Q_OBJECT

            zfs::FilterParamItem* item;
    public:

        ValidatedFilterItem(
            zfs::FilterParamItem* _item,
            QObject* parent = nullptr) :
            QItemDelegate(parent), item(_item) {
        }

        QWidget* createEditor(
            QWidget* parent, const QStyleOptionViewItem& options,
            const QModelIndex& index) const override;
    };
} // namespace zfstools
