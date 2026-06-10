
#include <iomanip>
#include <iostream>
#include <fstream>
#include <mutex>

#include <QDir>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QFileInfo>
#include <QCheckBox>
#include <QFile>
#include <QResizeEvent>
#include <QTextBrowser>
#include <QMessageBox>
#include <QSpinBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <lrcsocket/stream.h>
#ifdef _WINDOWS
#include <lrcsocket/simulation/scannersimulator.h> // new in 10.0
#endif

#ifdef USE_ZF_FILTER_DIALOG // disable this if you don't have the include and library
#include <LRV/zfstools/dlg/filterparam/DlgFilterParamImpl.h>
#endif
#include <zfcore/zfcore.h>
#include <zfcore/filename.h>
#include <zfs/filter/filter_param.h>
#include <zfs/filter/mixpix.h>
#include <zfs/zfsheader.h>
#include <zfs/zfs.h>
#include <zfs/lineheader.h>
#include <zfs/rainbow.h> // just for colorize pixel depending on intensity

const int plot_width = 400;
const int plot_height = 400;

#define PTS_STORE // just for debugging
#ifdef PTS_STORE
FILE* fpPTS{};
std::mutex mutPTS;
#endif

//############################################################################################################################

const QEvent::Type MY_CUSTOM_EVENT = static_cast<QEvent::Type>(QEvent::User + 1);

std::unique_ptr<zfs::ColorTable> lut;


QString str_pixel;
std::mutex mutStrPixel;


class MyPaintEvent : public QEvent
{
public:
    MyPaintEvent() : QEvent(MY_CUSTOM_EVENT) {}
    QImage im;
};

#ifdef USE_LINESTREAM

// zfs-header received
// send at start of a scan
// see zfs.ini for description of possible values
static void cbHeader(void* context, ZFSHeader& header)
{
//    const int typ = header.GetLong(1, 0);
//    const int pixel = header.GetLong(2, 0);

//    const auto start_time = header.GetLong(23002, 0);

// !!! do not change filter params here. They will be overwritten !!!
}

/**
called after receiving a zfs header and setting the default params depending on the device
Filter params ban be modified from the default here.
Later on you can change params on the fly.
*/
static void cbFilterParam(void* context, ZFSFilter& parm, const ZFSHeader& header)
{
    // recommended to modify filter params
#if 1
    {
        zfs::FilterParams fparams;
        fparams.buildStandardFilterParams(&header);
        fparams.useFrom(parm);
        //const bool currentValueEnlarge = fparams.getBool("enlarge");
        fparams.setBool("enlarge", false); // see pdf for key 'enlarge'
        fparams.setup(parm);
    }
#endif

    // There is a pdf which describes each filter params with the keys
    // The filter params may change from version to version. So in future there maybe new filters or params or default values are changing.
    // But you can ask for the default params.
#if 0
    {
        // one other way would be to read default params from a ini or xml file
        zfs::FilterParams fparams;
        fparams.buildStandardFilterParams(&header);
        std::wstring filterParamsIniFile(L"c:/fiter-template.ini");
        if (fparams.loadFilterParams(filterParamsIniFile))
        {
            fparams.setup(parm);
        }
    }
#endif
    

#if 0
    {
        // another way is to change params directly
        parm.bDiag = true;
        parm.enlarge = true;
    }
#endif

}


// new line received
static void cbXYZ(const lrcsocket::Stream &strm, void *context,
    const lrcsocket::Stream::LineData &ld,
    const lrcsocket::Stream::stream_pdata_t* pixels)
{
    if (!context)
        return;
    MainWindow *wnd = static_cast<MainWindow *>(context);
    try
    {
        wnd->paintXYZ(strm, ld, pixels);
    }
    catch (...) {}
}

// new file will be created
static std::wstring cbFilename(void* context, const std::wstring& firstFileName, const std::wstring& suggestion, uint32_t currentpart )
{
    return suggestion;
}

static void cbError(void* context, lrcsocket::Stream::err_t nr)
{
}

static void cbLine(void* context, const lrcsocket::Stream::LineData& ld)
{
}
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow),
    m_settings("ZF", "SampleCtrlSocket")
{
    m_ui->setupUi(this);

    loadSettings();

    connect(m_ui->pushButtonConnect, SIGNAL(pressed()), this, SLOT(btn_connect()));
    connect(m_ui->pushButtonDisconnect, SIGNAL(pressed()), this, SLOT(btn_disconnect()));
    connect(m_ui->pushButtonClear, SIGNAL(pressed()), this, SLOT(btn_clear()));

    connect(m_ui->pushButtonCmd1, SIGNAL(pressed()), this, SLOT(btn_cmd1()));
    connect(m_ui->pushButtonCmd2, SIGNAL(pressed()), this, SLOT(btn_cmd2()));
    connect(m_ui->pushButtonCmd3, SIGNAL(pressed()), this, SLOT(btn_cmd3()));
    connect(m_ui->pushButtonCmd4, SIGNAL(pressed()), this, SLOT(btn_cmd4()));
    connect(m_ui->pushButtonCmd5, SIGNAL(pressed()), this, SLOT(btn_cmd5()));
    connect(m_ui->pushButtonCmd6, SIGNAL(pressed()), this, SLOT(btn_cmd6()));
    connect(m_ui->pushButtonCmd7, SIGNAL(pressed()), this, SLOT(btn_cmd7()));
    connect(m_ui->pushButtonCmd8, SIGNAL(pressed()), this, SLOT(btn_cmd8()));

    connect(m_ui->pushButtonFilter, SIGNAL(pressed()), this, SLOT(btnFilter()));

    connect(m_ui->pushButtonZoomIn, SIGNAL(pressed()), this, SLOT(btnZoomIn()));
    connect(m_ui->pushButtonZoomOut, SIGNAL(pressed()), this, SLOT(btnZoomOut()));
    connect(m_ui->pushButtonUp, SIGNAL(pressed()), this, SLOT(btnUp()));
    connect(m_ui->pushButtonDown, SIGNAL(pressed()), this, SLOT(btnDown()));
    connect(m_ui->pushButtonLeft, SIGNAL(pressed()), this, SLOT(btnLeft()));
    connect(m_ui->pushButtonRight, SIGNAL(pressed()), this, SLOT(btnRight()));
    connect(m_ui->pushButtonHome, SIGNAL(pressed()), this, SLOT(btnHome()));

    connect(m_ui->pushButtonStartRec, SIGNAL(pressed()), this, SLOT(btnStartRec()));
    connect(m_ui->pushButtonStopRec, SIGNAL(pressed()), this, SLOT(btnStopRec()));

    connect(m_ui->checkBoxLife, SIGNAL(stateChanged(int)), this, SLOT(btnLife()));
    connect(m_ui->checkBoxFilter, SIGNAL(stateChanged(int)), this, SLOT(activateFilter()));
    connect(m_ui->checkBoxShowFilteredOut, SIGNAL(stateChanged(int)), this, SLOT(showFilterChanged()));

#ifdef USE_LINESTREAM
    const int cacheSizeLines = 5000;
    stream = new lrcsocket::Stream(cacheSizeLines);
    stream->setContext(this); ///< optional a context
    stream->XYZCallback(cbXYZ); ///< the most importand callback gives the life data stream filtered and calculated
    stream->filterParamCallback(cbFilterParam); ///< callback for modify filter params
    stream->headerCallback(cbHeader); ///< callback to get access to zfs-header values
    stream->fileNameCallback(cbFilename); ///< callback to modify filename for storing zfs files
    stream->errorCallback(cbError); ///< callback on error
    stream->lineCallback(cbLine); ///< raw line callback
#endif

    // timer to print out statistic
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
    m_timer->start( 250 );

    // create a color table
    lut = std::make_unique<zfs::ColorTable>(1000);

#ifdef PTS_STORE
    connect(m_ui->pushButtonPTS, SIGNAL(clicked(bool)), this, SLOT(clickedPTS(bool)));
#endif
}

MainWindow::~MainWindow()
{
#ifdef USE_LINESTREAM
    delete stream;
#endif

    saveSettings();

    delete m_ui;
}


void MainWindow::closeEvent(QCloseEvent *ev)
{
#ifdef USE_LINESTREAM
    stream->XYZCallback(nullptr);
#endif

    QMainWindow::closeEvent(ev);
}

void MainWindow::btn_connect()
{
    const QString ip = m_ui->lineEditIP->text();


    // possibility to do a scanner simulation. Use addr localhost for that
    // new in 10.0
#ifdef _WINDOWS
    if (ip == "127.0.0.1" || ip == "localhost")
    {
        using namespace zfsocket::simulation;
        // you have to connect to localhost to allow a scanner simulation
        ScannerSimulatorParam prm("9020");
        startScannerSimulator( prm );
    }
#endif

    m_life = true;
    m_ui->checkBoxLife->setChecked(true);

    if ( ip.length()) {
#ifdef USE_INTERFACE
        if (myif->Connect(ip.toStdString().data(), PORT_COMMAND))
            m_ui->textBrowserResult->append("connected to command interface");
        else
            m_ui->textBrowserResult->append("connect to command interface failed");
#endif
    } else
        m_ui->textBrowserResult->append(
            "port or addr not defined for command interface.");

#ifdef USE_LINESTREAM
    // if you have more than one scanner which stream on the same port, you can add the ip addr to the connect call, otherwise let it empty to get stream from everywhere

    if ( m_ui->radioButtonTCP->isChecked())
    {
        // !!! connect via tcp. In this case the scancommand must have the '-linestream tcpzip'
        // port 6105 is used by the scanner and can not be changed (Nov 2022)
        // new since Nov 2022
        // scanner device need support for that. For example 9020 since firmware 9.3.3
        if (stream->connectTCP(PORT_TCPZIP, ip.toStdString()))
            m_ui->textBrowserResult->append("connected to tcp line stream interface");
        else
            m_ui->textBrowserResult->append("connect to tcp line stream interface failed");
    }
    else if (m_ui->radioButtonUDP->isChecked())
    {
        // !!! connect via UDP. In this case '-linestream all' is the only possible way
        // all devices have that support
        if (stream->connectUDP(PORT_LINEDATA, ip.toStdString()))
            m_ui->textBrowserResult->append("connected to udp line stream interface");
        else
            m_ui->textBrowserResult->append("connect to udp line stream interface failed");
    }
    // this is a obsolete way which does not work for all devices
    else if (m_ui->radioButtonXYZ->isChecked())
    {
        // !!! connect via TCP. In this case you have to use'-linestream xyz'
        // new since 2023 version 10.0
        if (stream->connectXYZ(PORT_TCPZIP, ip.toStdString()))
            m_ui->textBrowserResult->append("connected to xyz line stream interface");
        else
            m_ui->textBrowserResult->append("connect to xyz line stream interface failed");
    }
#endif



    m_ui->radioButtonTCP->setEnabled(false);
    m_ui->radioButtonUDP->setEnabled(false);
    m_ui->radioButtonXYZ->setEnabled(false);
}

void MainWindow::btn_disconnect()
{
#ifdef PTS_STORE
    closePTS();
#endif

#ifdef USE_INTERFACE
    myif->Disconnect();
#endif

#ifdef USE_LINESTREAM
    stream->disconnect();
#endif

    clear();


    m_ui->radioButtonTCP->setEnabled(true);
    m_ui->radioButtonUDP->setEnabled(true);
    m_ui->radioButtonXYZ->setEnabled(true);

#ifdef _WINDOWS
    zfsocket::simulation::stopScannerSimulator(); // new in 10.0
#endif
}

void MainWindow::clear()
{
    m_life = false;
    m_ui->textBrowserResult->clear();
    m_ui->textBrowserEvent->clear();
    m_ui->textBrowserDebug->clear();
    m_ui->checkBoxLife->setChecked(true);
    m_ui->checkBoxFilter->setChecked(true);
    m_ui->checkBoxShowFilteredOut->setChecked(false);
    m_ui->labelGraphic->setText("");
}

void MainWindow::btn_clear()
{
    m_ui->textBrowserResult->clear();
    m_ui->textBrowserEvent->clear();
}

void MainWindow::btn_cmd1()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd1->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd2()
{
#ifdef USE_INTERFACE
    const QString cmd = m_ui->lineEditCmd2->text();
    const std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd3()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd3->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd4()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd4->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd5()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd5->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd6()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd6->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(QString("command:'%1' result:'%2'")
        .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd7()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd7->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::btn_cmd8()
{
#ifdef USE_INTERFACE
    QString cmd = m_ui->lineEditCmd8->text();
    std::string result = myif->Cmd(cmd.toUtf8().data());
    m_ui->textBrowserResult->append(
        QString("command:'%1' result:'%2'")
            .arg(cmd, QString::fromUtf8(result.c_str())));
#endif
}

void MainWindow::applyFilter( const std::shared_ptr<zfs::FilterParams> params)
{
#ifdef USE_LINESTREAM
    stream->setFilter(params);
#endif
}


// This allows to redefine the filter params while streaming.
// New params will be used for next line processed.
// At the beginning of a stream the callback is called to setup the initial params.
// Later on you can use that code to change params on the fly
void MainWindow::btnFilter()
{
#ifdef USE_LINESTREAM
#ifdef USE_ZF_FILTER_DIALOG

    // base translator only shows english texts. To have translated text
    // another implementation if the BaseFilterTranslator has to be provided

    const ZFSFilter* zprm = stream->getFilterParams();
    if (zprm == nullptr) return;

    auto params = std::make_shared<zfs::FilterParams>(zprm->setupPrm);
    params->useFrom(*zprm);
    zfstools::DlgFilterParamImpl dlg(this, params);
    dlg.enableApply();
    connect(
        &dlg, &zfstools::DlgFilterParamImpl::applyFilter, this,
        [=](std::shared_ptr<zfs::FilterParams> params)
        {
            applyFilter(params);
        });

    if (dlg.exec() == QDialog::Accepted) {
        ZFSFilter zflt;
        params->setup(zflt);
        stream->setFilter(zflt); // the filters will be used for the next line processed
    }
#endif // USE_ZF_FILTER_DIALOG
#endif // USE_LINESTREAM
}





static double scale = 3.; // scaling [meter]
static int xoff = 0;
static int yoff = 0;


#ifdef USE_LINESTREAM
void MainWindow::paintXYZ(const lrcsocket::Stream& strm,
    const lrcsocket::Stream::LineData& ld,
    const lrcsocket::Stream::stream_pdata_t* pixels)
{
    if (m_life == false)
        return;

    m_lifeSubsample--;
    if (m_lifeSubsample <= 0)
        m_lifeSubsample = m_ui->spinBoxSub->value();
    else
        return;

    auto ev = new MyPaintEvent();
    ev->im = QImage(plot_width, plot_height, QImage::Format_RGB32);

    {
        QImage& im = ev->im;
        QPainter painter(&im);

        QColor background(130, 130, 255);
        if (ld.calibrated == false)
            background = QColor(255, 0, 0); // uncalibrated
        else if (ld.filtered == false)
            background = QColor(255, 192, 192); // if not filtered, change background
        im.fill(background);
        const int wd = im.width();      // output image size
        const int hg = im.height();
        const int mx = wd / 2; // center
        const int my = hg / 2;

        // the line header access of the stream
        zfs::LineHeader lh((char*)ld.lineHeader, ld.lhsz);
        const auto nr = lh.linenumber(); ///< the linenumber of the line
        const auto cnt1 = lh.cnt1(); ///< counter 1 of the line. Typically the PPS
        const auto lineTimeMS = lh.time(); ///< not accurate, just for cross checking

             // grid
        for (int i = 0; i < hg; i++)
            im.setPixel(mx + xoff, i + yoff, QColor(192, 192, 192).rgb());
        for (int i = 0; i < wd; i++)
            im.setPixel(i + xoff, my + yoff, QColor(192, 192, 192).rgb());

        if (pixels)
        {
            for (auto i = 0u; i < ld.pixel; i++)
            {
                const auto& px = pixels[i];
                const auto y = static_cast<double>(px.xyz.getY());
                const auto z = static_cast<double>(px.xyz.getZ());
                const int xp = mx + static_cast<int>(y / scale * mx + 0.5) + xoff;
                const int yp = my - static_cast<int>(z / scale * my + 0.5) + yoff;

                if (xp >= 0 && yp >= 0 && xp < wd && yp < hg)
                {
                    QColor col;

                    /// this byte has some more information about counters like at what pixel the counter changes in the line
                    const auto addition_byte_of_pixe = px.add; 

                    const bool flt = stream->isFilterActive();
                    if (flt)
                    {
                        // check if the point was filtered out or not
                        if (px.mask & zfs::PDataRaw::MASK_FILTERED_OUT)
                        {
                            col = QColor(192, 0, 0); // masked out by filter
                            if (m_showFilteredOut == false)
                                continue;
                        }
                        else if (px.rf < ld.minRf)
                        {
                            col = QColor(255, 0, 0); // low intensity pixels red
                            if (m_showFilteredOut == false)
                                continue;
                        }
                        else
                        {
                            const auto rfn = px.rfNorm;
                            if (rfn > 1.) // overloaded
                            {
                                col = QColor(255, 255, 0); // overloaded yellow
                            }
                            else
                            {
                                const int c = static_cast<int>(px.RfNorm1() * 255); // use normalized value for gray view
                                col = QColor(c, c, c);
                            }
                        }
                    }
                    else
                    {
#if 1
                        // view intensity as rainbow
                        const auto c = (*lut)(static_cast<int>(px.RfNorm1() * lut->size()));
                        col = QColor(c.r(), c.g(), c.b());
#else
                        // view as gray
                        const int c = static_cast<int>(px.RfNorm1() * 255); // use normalized value for gray view
                        col = QColor(c, c, c);
#endif
                    }

                    im.setPixel(xp, yp, col.rgb());
#if 1 // big points
                    im.setPixel(xp + 1, yp, col.rgb());
                    im.setPixel(xp, yp + 1, col.rgb());
                    im.setPixel(xp + 1, yp + 1, col.rgb());
#endif
                }
            }

            // print out data of the selected pixel
            {
                std::unique_lock<std::mutex> lk(mutStrPixel);
                str_pixel.clear();
                if (ld.pixel)
                {
                    const auto pixel_index_print = std::min(static_cast<uint32_t>(m_ui->spinBoxPixel->value()), ld.pixel - 1);
                    const auto& px = pixels[pixel_index_print];
                    str_pixel = QString("pixel %1 of %2\n").arg(pixel_index_print).arg(ld.pixel);
                    str_pixel += QString("rf: %1 rfnorm:%2\n").arg(px.rf).arg(px.rfNorm,0,'f',4);
                    str_pixel += QString("rg: %1\n").arg(px.rg,0,'f',3);
                    str_pixel += QString("vert: %1 horz:%2\n").arg(px.angle.first, 0, 'f', 3).arg(px.angle.second, 0, 'f', 3);
                    str_pixel += QString("xyz: %1 %2 %3\n").arg(px.xyz.x(), 0, 'f', 3).arg(px.xyz.y(), 0, 'f', 3).arg(px.xyz.z(), 0, 'f', 3);
                    str_pixel += QString("vert: %1deg\n").arg(px.getVerticalRad() * 180. / M_PI);
                    str_pixel += QString("add: %1\n").arg(px.add);
                    str_pixel += QString("mask: %1 filterID: %2\n").arg(px.mask).arg(px.filterID);
                }
            }
        }

        painter.setPen(Qt::darkBlue);
        painter.setFont(QFont("Times", 8));
        painter.drawText(1, 15, QString("ref rf:%1 rg:%2 off:%3").arg(ld.refRf, 0, 'f', 0).arg(ld.refRg, 0, 'f', 4).arg(ld.rgOffsetM, 0, 'f', 4)); // reference measurement
        painter.drawText(1, 40, QString::fromStdWString(ld.filterReport));

        // print some values from line header
        painter.drawText(1, 50 + 1 * 15, QString("horz:%1").arg(lh.encHorz()));
        painter.drawText(1, 50 + 2 * 15, QString("cnt1:%1").arg(lh.cnt1()));
        painter.drawText(1, 50 + 3 * 15, QString("cnt2:%1").arg(lh.cnt2()));
        painter.drawText(1, 50 + 4 * 15, QString("cnt3:%1").arg(lh.cnt3()));
        if( lh.hasPixelCounter() )
        painter.drawText(1, 50 + 5 * 15, QString("npx:%1").arg(lh.pixelCounter()));
    }

    // just for debugging. Print out the datastream as a pts file
    // this is not possible in productive environment because to slow
#ifdef PTS_STORE
    {
        std::unique_lock<std::mutex> lk(mutPTS);
        if (fpPTS)
        {
            for (auto i = 0u; i < ld.pixel; i++)
            {
                const auto& px = pixels[i];
                if ((px.mask & zfs::PDataRaw::MASK_FILTERED_OUT) == 0 && px.rf >= ld.minRf && px.filterID == 0)
                {
                    double x = 0;// px.x() + (double)nr / 100;
                    fprintf(fpPTS, "%lf %lf %lf %.0lf\n", x, px.y(), px.z(), px.RfNorm1() * 4096.);
                }
            }
        }
    }
#endif

    QApplication::postEvent(this, ev);
}
#endif

void MainWindow::loadSettings()
{
    const QString addr = m_settings.value("addr", "localhost").toString();
    m_ui->lineEditIP->setText(addr);

    m_ui->lineEditCmd1->setText(m_settings.value("cmd1", "stop").toString());
    m_ui->lineEditCmd2->setText(m_settings.value("cmd2", "version").toString());
    m_ui->lineEditCmd3->setText(m_settings.value("cmd3", "abort").toString());
    m_ui->lineEditCmd4->setText(m_settings.value("cmd4", "ignore").toString());
    m_ui->lineEditCmd5->setText(m_settings.value("cmd5", "scan -profiler -resolution h -quality n -sel full -file test -path test -writing 0 -counterbyte -filesize 250mb -linestream all -statusstream -ip YOUR_LOCAL_IP" ).toString());
    m_ui->lineEditCmd6->setText(m_settings.value("cmd6", "scan -profiler -resolution h -quality n -sel full -file test -path test -writing 0 -counterbyte -filesize 250mb -linestream tcpzip -statusstream -ip YOUR_LOCAL_IP" ).toString());
    m_ui->lineEditCmd7->setText(m_settings.value("cmd7", "scan -profiler -resolution h -quality n -sel full -file test -path test -writing 0 -counterbyte -filesize 250mb -linestream xyz -statusstream -ip YOUR_LOCAL_IP" ).toString());
    m_ui->lineEditCmd8->setText(m_settings.value("cmd8", "").toString());

    const auto storage_path = QDir::homePath();
    m_ui->lineEditPath->setText(m_settings.value("path", storage_path).toString());

    m_ui->lineEditFile->setText(m_settings.value("file", "profiler").toString());
}

void MainWindow::saveSettings()
{
    m_settings.setValue("addr", m_ui->lineEditIP->text());

    m_settings.setValue("cmd1", m_ui->lineEditCmd1->text());
    m_settings.setValue("cmd2", m_ui->lineEditCmd2->text());
    m_settings.setValue("cmd3", m_ui->lineEditCmd3->text());
    m_settings.setValue("cmd4", m_ui->lineEditCmd4->text());
    m_settings.setValue("cmd5", m_ui->lineEditCmd5->text());
    m_settings.setValue("cmd6", m_ui->lineEditCmd6->text());
    m_settings.setValue("cmd7", m_ui->lineEditCmd7->text());
    m_settings.setValue("cmd8", m_ui->lineEditCmd8->text());

    m_settings.setValue("path", m_ui->lineEditPath->text());
    m_settings.setValue("file", m_ui->lineEditFile->text());
}

//============================================================

void MainWindow::btnZoomIn()
{
    if (scale>0.5)
        scale -= 0.5;
}

void MainWindow::btnZoomOut()
{
    scale += 0.5;
}

void MainWindow::btnUp()
{
    yoff -= 5;
}
void MainWindow::btnDown()
{
    yoff += 5;
}
void MainWindow::btnLeft()
{
    xoff -= 5;
}
void MainWindow::btnRight()
{
    xoff += 5;
}
void MainWindow::btnHome()
{
    xoff = 0;
    yoff = 0;
    scale = 3;
}

//============================================================

// print statistic and informations
void MainWindow::timerEvent()
{
#ifdef USE_INTERFACE
    const int st = myif->GetStatusViaCmd();
    if (myif->IsConnected())
    {
        if ((st & 1) == 0) return; // at least bit 1 has to be set, otherwise try again on next timer event

        QString sst;
        if (st & 2) sst += "stream ";
        if (st & 4) sst += "program ";
        if (st & 8) sst += "error ";
        if (st & 0x10) sst += "write ";
        if (st == 1) sst = "ready"; // if just 1, than ready for new scan commands
        m_ui->labelScannerStatus->setText(QString("status:%1 '%2'").arg(st).arg(sst));

        const std::string prg = myif->Cmd("prg");
        if (prg.length())
        {
            const std::string info = LRCInterface::ParseXMLAttrOnly(prg, "prg", "info");
            m_ui->labelPrg->setText(QString::fromStdString(info));
        }

        /*
        bool wr;
        uint64_t fsize;
        uint32_t part;
        stream->recordingInfo(wr, fsize, part);
        m_ui->labelRecording->setText(
            QString("recording:%1 file-size:%2MB part:%3")
                .arg(wr ? "yes" : "no")
                .arg(double(fsize) / 1024. / 1024., 0, 'f', 1)
                .arg(part));
                */

        const auto stat = stream->getStatistic();
        QString st;
        st += QString("valid: %1 filter: %2  wr: %3\n").arg(stat.valid).arg(stat.filterPossible).arg(stat.recordPossible);
        st += QString("addr: %1\n").arg(stat.addr.c_str());
        st += QString("nErrors: %1 lastErrorCode: %2\n").arg(stat.nErrors).arg(stat.lastErrorCode);
        st += QString("missedLines: %1 trashed: %2\n").arg(stat.missedLines).arg(stat.trashed);
        st += QString("datatype: %1 nHeaderReceived: %2\n").arg(stat.datatype).arg(stat.nHeaderReceived);
        st += QString("lines: %1 pixel: %2\n").arg(stat.lines).arg(stat.pixel);
//        st += QString("pixelFilteredOut: %1\n").arg(stat.pixelFilteredOut);
        st += QString("flagsOfLineheader: %1 '%2'\n").arg(stat.flagsOfLineheader).arg( QString::fromStdString( zfs::LineheaderFlag2String(stat.flagsOfLineheader) ));
        st += QString("lineHeaderTime: %1\n").arg(stat.lineHeaderTime);
        st += QString("counterL/1/2: %1 %2 %3\n").arg(stat.counterL).arg(stat.counter1).arg(stat.counter2);
        // file recording
        st += QString("started: %1  writingToFile: %2\n").arg(stat.started).arg(stat.writingToFile);
        if (stat.writingToFile) // if writing to file, some more information about that
        {
            st += QString("currentPart: %1  linesWrittenToFile: %2\n").arg(stat.currentPart).arg(stat.linesWrittenToFile);
            st += QString("mbCurrentFile: %1mb  all: %2mb\n").arg(stat.mbCurrentFile).arg(stat.mbAllParts);
        }
        // performance
        st += QString("lps: %1   %2ms\n").arg(stat.lps).arg(stat.lps ? 1000./stat.lps : 0);
        st += QString("cache: %1 of %2\n").arg(stat.cacheFree).arg(stat.cacheSize);
        st += QString("performance filter: %1ms cb: %2ms calc: %3ms\n").arg(stat.perfFilter).arg(stat.perfXYZ).arg(stat.perfBackground);
        st += QString("datarate: %1k/sec\n").arg(stat.datarate);
        st += QString("elevation offset: %1deg\n").arg(stat.elevationOffsetDeg);

        QString available = stat.available & zfs::PDatas<zfs::PDataDetail>::A_REFERENCE ? "ref," : "";
        available += stat.available & zfs::PDatas<zfs::PDataDetail>::A_RFNORM ? "rfnorm," : "";
        st += QString("available %1\n").arg(available);

        m_ui->textBrowserDebug->setText(st);
    }
    else
    {
        m_ui->labelScannerStatus->setText("scanner not connected");
        m_ui->labelPrg->setText("");
    }
#else
    m_ui->labelScannerStatus->setText("");
#endif


    {
        std::unique_lock<std::mutex> lk(mutStrPixel);
        m_ui->textBrowserPixel->setText(str_pixel);
    }
}

void MainWindow::btnStartRec()
{
#ifdef USE_LINESTREAM
#ifdef _DEBUG
    stream->setSplitSize(10 * 1024 * 1024); // for testing use small size
#else
    stream->setSplitSize(100 * 1024 * 1024); // for testing use small size
#endif

    zf::FileName fn( m_ui->lineEditPath->text().toStdWString() );
    fn.AddName( m_ui->lineEditFile->text().toStdWString() );
    fn.AddExt( ".zfs" );
    if (stream->startFile(fn.WString()) == false )
    {
        QMessageBox::critical(this, "Error", "Can't start recording");
    }
#endif
}

void MainWindow::btnStopRec()
{
#ifdef USE_LINESTREAM
    stream->closeFile();
#endif
}


void MainWindow::customEvent(QEvent* event)
{
    if (event->type() == MY_CUSTOM_EVENT)
    {
        MyPaintEvent *my = static_cast<MyPaintEvent *>(event);
        if (!my->im.isNull())
        {
            const int w = my->im.width();
            const int h = my->im.height();
            m_ui->labelGraphic->setPixmap( QPixmap::fromImage(my->im).scaled(w, h) );
        }
    }
}

void MainWindow::activateFilter()
{
    stream->activateFilter(m_ui->checkBoxFilter->isChecked());
}

void MainWindow::showFilterChanged()
{
	m_showFilteredOut = m_ui->checkBoxShowFilteredOut->isChecked();
}

// freeze the screen update
void MainWindow::btnLife()
{
    m_life = !m_life;
}

//============================================================

void MainWindow::closePTS()
{
#ifdef PTS_STORE
    std::unique_lock<std::mutex> lk(mutPTS);
    if (fpPTS)
    {
        fclose(fpPTS);
        fpPTS = nullptr;
    }
#endif
}
void MainWindow::clickedPTS(bool st)
{
#ifdef PTS_STORE
    if (st)
    {
        // open a file for streaming to a PTS file

        closePTS();

        zf::FileName pts(m_ui->lineEditPath->text().toStdWString());
        pts.AddName(m_ui->lineEditFile->text().toStdWString());
        pts.AddExt(".streaming.pts");
        fpPTS = fopen( pts.utf8().c_str(), "wt");
    }
    else
    {
        // close the pts file
        closePTS();
    }
#endif
}