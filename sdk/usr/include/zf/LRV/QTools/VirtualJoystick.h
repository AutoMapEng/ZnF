#ifndef _VIRTUALJOYSTICK_H_
#define _VIRTUALJOYSTICK_H_

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QGraphicsItem>
#include <QObject>
#include <QWidget>
#include <QColor>
#include <QBrush>
#include <QString>
SWITCH_WARNINGS_ON

#include "QToolsApi.h"

class QMouseEvent;
class QPaintEvent;


/**
* Brief Class to generates the virtual joystick
*/
class QTOOLS_API VirtualJoystick : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QBrush backgroundCircleColor1 READ getBackgroundCircleColor1 WRITE setBackgroundCircleColor1)
    Q_PROPERTY(QBrush backgroundCircleColor2 READ getBackgroundCircleColor2 WRITE setBackgroundCircleColor2)
    Q_PROPERTY(QString backgroundCircleImage READ getBackgroundCircleImage WRITE setBackgroundCircleImage)

signals:
    void changeColor();

private:
    QBrush backgroundCircleColor1;
    QBrush backgroundCircleColor2;
    QString backgroundCircleImage;

    QPixmap pixmap;

public:
    /**
    * Breif class constructor to init the parameters
    * @param widgetWidth double this is the size of the controller scene
    */
    VirtualJoystick(QWidget* parent, double widgetWidth);
    ~VirtualJoystick();

    QBrush getBackgroundCircleColor1() { return backgroundCircleColor1; }
    void setBackgroundCircleColor1(QBrush brush) { backgroundCircleColor1 = brush; }

    QBrush getBackgroundCircleColor2() { return backgroundCircleColor2; }
    void setBackgroundCircleColor2(QBrush brush) { backgroundCircleColor2 = brush; }

    QString getBackgroundCircleImage() { return backgroundCircleImage; }
    void setBackgroundCircleImage(QString image) { backgroundCircleImage = image; }

    /**
    * Breif gets the x mpuse position
    */
    double getX();
    /**
    * Breif gets the y mpuse position
    */ 
    double getY();

    public slots:
        /**
        * Brief slot to normalize the stick position to be between -1 and 1
        * this slots is connected to a timer (10 times per second)
        */
        void normalizeXY();

signals:
        /**
        * Brief emits the normalized x and y of of the stick
        */
        void sendXY(double x, double y);

protected:
        virtual void paintEvent(QPaintEvent* event);

private:
    //flag for pressed mouse bottun
    bool mouseState_;
    //mouse positioin
    double mouseX_;
    double mouseY_;
    //radius of the joystick
    double radius_;

    /**
    * Brief paintGui create the visual controller
    */
    void paintGui();

protected:
    /**
    * Brief mousePressEvent handles the mouse event
    */
    void mousePressEvent(QMouseEvent *event);
    /**
    * Brief mouseReleaseEvent handles the mouse event
    */ 
    void mouseReleaseEvent(QMouseEvent *event);
    /**
    * Brief mouseMoveEvent handles the mouse event
    */
    void mouseMoveEvent(QMouseEvent *event);
}; 

#endif //_VIRTUALJOYSTICK_H_
