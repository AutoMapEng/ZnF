#ifndef BUTTONMENU_H
#define BUTTONMENU_H

#include "QToolsApi.h"

#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QBoxLayout>
#include <QWidget>
#include <QFrame>
#include <QPoint>
#include <QList>
#include <QToolButton>
#include <QColor>

#include <QEvent>
#include <QGestureEvent>

#include <QToolBar>
#include <QMainWindow>
#include <QObject>

#include <boost/scoped_ptr.hpp>
SWITCH_WARNINGS_ON

#include "qtools_enums.h"

class QAction;
class QToolBar;
class QMainWindow;
class QToolButton;

namespace zfqtools {

/**
\brief Collection of values that influence the appearance of a QAction.
*/
    class QTOOLS_API ActionParameter {
    public:
        QString svgIconFilename = "";
        QString toolTip = "";
        QString text = "";
        bool addDropShadow = false;

        ActionParameter(const QString& svgIconFilenameArg, 
            const QString& toolTipArg = QString(), const QString& text = "",
            bool dropShadow = false)
            : svgIconFilename(svgIconFilenameArg), toolTip(toolTipArg), 
            text(text), addDropShadow(dropShadow) {
        }
    };

    /**
    \brief This class only defines another type for a QToolButton to make it possible to set it to another stylesheet.
    */
    class ButtonMenuToolButton : public QToolButton {
        Q_OBJECT  // needed to make stylesheets work

    public: ButtonMenuToolButton(QWidget* parent)
            : QToolButton(parent) {
            setObjectName("ButtonMenuToolButton");

            grabGesture(Qt::TapAndHoldGesture);
        }

    };

    class CloseButtonMenuEventFilter;
    class ButtonMenuLayout;

    /**
    \brief The ButtonMenu provides a "floating" pull down menu for tool buttons.

    The purpose of the ButtonMenu is to save space in the GUI. Touch-input support often creates the need
    for more buttons which might take to much space in the toolbar. The ButtonMenu provides an easy way
    to group multiple buttons under one permanently visible button in the toolbar.

    Unlike the QMenu the ButtonMenu does not display additional text for the buttons. It lets the user
    choose the alignment of the buttons (horizontally/vertically).

    The style of the buttons will automatically be set to the styles of QToolButtons in the application.

    # Usage #

    \code{.cpp}

    qtools::ButtonMenu* myButtonMenu = new qtools::ButtonMenu( qtools::ActionParameter(":/PixMaps/iconForMenu.svg"), myToolBar, mainWnd);

    QAction* action = myButtonMenu->addAction( ":/PixMaps/iconForButtonInMenu.svg", true);

    // You can add the action to groups or other menus or whatnot here.

    \endcode


    # Possible pitfalls #

    - For now it is not guaranteed that the menu will always fit on your screen. If you experience
    problems here, you need to add your special case to ButtonMenu::shiftMenuPositionIfItIsTooCloseToApplicationBorder()

    - To make keyboard shortcuts work for the added actions when the menu is not open, the actions are added to
    the application window. It is not known if this has any side effects.


    */
#pragma warning(push)
#pragma warning(disable: 4251 )
    class QTOOLS_API ButtonMenu : public QObject {
        Q_OBJECT

    private:
        QAction* openAction;  ///< The action that is used to open the ButtonMenu.
        QToolBar* toolBar;
        QMainWindow* parentWindow;

        OpenOffset openOffset;
        const int offsetToCallingWidget;  ///< Determines the size of the gap between the Toolbar that opened the menu and the ButtonMenu in pixel.

        int spacing;        ///< The space [pixel] between the buttons and the widget border.
        bool closeOnClick;  ///< Flag that signals that the menu should be closed when a button in the menu is clicked.

        CloseButtonMenuEventFilter* closeMenuEventFilter;

        QList<QToolButton*> buttons;
        bool buttonSizeInitiated;

        QFrame* buttonFrame;

        // boost::scoped_ptr<ButtonMenuLayout> layout;
        QBoxLayout* layout;

        static const double spacingFactor;  ///< This factor is multiplied with the QToolButton width with to determine the spacing between buttons in the menu.

        bool updateIconOnClick = false;
        static bool openIndicatorLeft;

    public:
        /**
        \param params Parameter that determine the icon and tooltip of the button that opens the menu.
        \param toolbar The toolbar to which the button that opens the menu will be added.
        \param parent The QMainWindow that will be used as a parent for the button widgets.
        */
        ButtonMenu(const ActionParameter& params, QToolBar* toolbar, QMainWindow* parent, QWidget* parentWidget = NULL);

        virtual ~ButtonMenu();

        QAction* getOpenAction();  ///< Returns the action that is used to open the menu.

        void setOpenOffset(OpenOffset offset) { openOffset = offset; }
        void setButtonAlignment(QBoxLayout::Direction alignment);

        void closeOnButtonClick(bool b);  ///< If set to false, clicking a button in the menu will not close the menu.

        QAction* addAction(QString iconFilename, bool checkable);  ///< Ownership of the QAction stays with the ButtonMenu.
        QAction* addAction(QAction* action);                       /**< Does not take ownership of the action.*/

        void addSpacer();

        void enhanceIcon(const ActionParameter& params);
        void enhanceIcon(const QIcon& icon, const QSize& size);
        void enhanceIcon(const QPixmap& pixAction, QPixmap& pixActionExtendet);

        void setUpdateIconOnClick(bool value);

        static void setOpenIndicatorLeft(bool value);

        static bool isOpenIndicatorLeft();


    signals:
        void signalActionTriggered(QAction* triggeredAction);
        void signalMenuVisibilityChanged(bool visibility);

    private slots:
        void emitActionTriggeredSignalAndCloseMenuIfNecessary();
        void slotShow(bool b = true);  ///< This will display the ButtonMenu at a position close to the sender of the connected signal.
        void closeMenu();

    private:
        void init();
        void addDropShadow(QWidget* tempWidget) const;

        void placeMenuCloseToToolbarAndToolButton();
        QPoint getOffsetToolBarBorderToMenuButtons();
        QPoint getOffsetMenuButtonToToolBarBorder();
        QPoint getGlobalToolButtonPosition();
        void shiftMenuPositionIfItIsTooCloseToApplicationBorder(QPoint& menuPosition);
        bool checkMenuColidesWithRightBorder(QPoint position);
        bool checkMenuColidesWithLowerBorder(QPoint position);
        QPoint getParentWindowGlobalPosition();
        QToolButton* getMenuOpenButton();
        void setSpacing();

        /**
        Set the alignment perpendicular to the toolbar and the offset into the window that contains the toolbar.
        This is the default setting when callingActionArg in the constructor belongs to a toolbar.
        */
        void setOffsetSpacingAndAlignmentWithToolBar();

        void installCloseButtonMenuEventFilter(bool b);
        /**
        Sets the visibility of the buttons that have visible actions.
        Buttons that belong to actions that have been made invisible (maybe because of missing licenses)
        will therefore not be made visible.
        */
        void setActiveButtonsVisible(bool b);
    };
#pragma warning(pop)


// ###############################################################################
/**
\brief Event filter for the ButtonMenu that intercepts user events.

This event filter is used by the ButtonMenu to intercept MouseButtonPress events
and keyboard events to close itself when the user clicks somewhere else. Events for the open
button and and the buttons in the menu are passed on.
*/
    class CloseButtonMenuEventFilter : public QObject {
        Q_OBJECT

            QList<QObject*> objectsThatStillReceiveEvents;

    public:
        CloseButtonMenuEventFilter(QObject* parent) : QObject(parent) {}

        void addObjectThatCanReceiveEvents(QObject* object);

    signals:
        void menueClosingEventOccurred();

    protected:
        virtual bool eventFilter(QObject* obj, QEvent* event);
    };
}

#endif  // BUTTONMENU_H
