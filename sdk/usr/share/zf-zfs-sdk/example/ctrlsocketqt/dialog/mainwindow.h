#pragma once

#include <QMainWindow>
#include <QSettings>
#include <QTimer>

#include "../if.h"

#include <lrcsocket/stream.h>

namespace Ui {
class MainWindow;
}
class QImage;
namespace zfs { class FilterParams; }



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
#ifdef USE_INTERFACE
    zf::ctrl::MyLRC* myif{};
#endif
#ifdef USE_LINESTREAM
        lrcsocket::Stream* stream = nullptr;
#endif

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

#ifdef USE_LINESTREAM
    void paintXYZ(const lrcsocket::Stream& strm, const lrcsocket::Stream::LineData& ld, const lrcsocket::Stream::stream_pdata_t* pixels);
#endif

protected:
    virtual void closeEvent( QCloseEvent* ev ) override;

    // load the user settings
    void loadSettings();
    // store the user settings
    void saveSettings();

    void customEvent(QEvent* event) override;

    // clear outputs
    void clear();

    // close the PTS file
    void closePTS();

private slots:
	void btn_connect();
    void btn_disconnect();
    void btn_clear();
	void btn_cmd1();
	void btn_cmd2();
	void btn_cmd3();
	void btn_cmd4();
	void btn_cmd5();
    void btn_cmd6();
    void btn_cmd7();
    void btn_cmd8();
    void btnFilter();
    void btnZoomIn();
    void btnZoomOut();
    void btnUp();
    void btnDown();
    void btnLeft();
    void btnRight();
    void btnHome();
    void btnStartRec();
    void btnStopRec();

    void activateFilter();
	void showFilterChanged();
	void applyFilter(const std::shared_ptr<zfs::FilterParams>);
    void btnLife();

    void timerEvent();

    void clickedPTS(bool);

private:
    Ui::MainWindow *m_ui{};
    QSettings m_settings;
    QTimer* m_timer{};
    bool m_showFilteredOut{false};
    bool m_life{ true };
    int m_lifeSubsample{};
};
