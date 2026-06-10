#ifndef BUTTONMENULAYOUT_H
#define BUTTONMENULAYOUT_H

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QList>
#include <QPoint>
#include <QToolButton>
SWITCH_WARNINGS_ON

#include <vector>

#include "qtools_enums.h"

class QWidget;


namespace zfqtools
{

    /**
    \brief This class is responsible for arranging the buttons in the ButtonMenu.
    */
    class ButtonMenuLayout
    {
        ButtonAlignment buttonAlignment;

        QList<QToolButton*> buttons;
        std::vector<int> spacerIndexes;     ///< This indicates after which widgets in buttons the layout is supposed to place spacers.

        QPoint upperLeftCorner;
        int spacing;

        static const double spacerSizeFactor;   ///< The size of a spacer is spacerSizeFactor * buttonWidth or buttonHeight;

    public:
        ButtonMenuLayout();

        void setPosition(QPoint position);    ///< Position of the upper left corner of the buttons in coordinates of the parent widget of the buttons.
        int getHeight();
        int getWidth();

        void addButton( QToolButton* button);   ///< The layout does not take ownership
        void addSpacer();                   /**< Use this to visually group buttons. This adds a larger gap between buttons.*/

        void setAlignment(ButtonAlignment alignment);
        void setSpacing(int spacing);       ///< Set the gap size between buttons.

    private:
        void updateWidgetPositions();
        QToolButton* getFirstActiveButton();
        QToolButton* getLastActiveButton();
    };


}


#endif // !BUTTONMENULAYOUT_H
