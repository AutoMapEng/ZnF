#pragma once

#include <string>
#include <vector>

#include <QObject>

#include <QByteArray>

#include "sens.h"
#include "zfnav_api.h"

class QUdpSocket;

/**
 * @brief The GpsUdp class implements a udp receiver for GPS messages
 * ZFGPSSimulator.exe Sendet die Messages.
 */
class ZFNAV_API NavGpsUdp : public QObject
{
  Q_OBJECT

public:
    bool newData;
    enum { ST_UNDEF, ST_INIT, ST_CONNECTED, ST_DATA, ST_DISABLED, ST_CLOSED };
    int status{};
    bool enable{};

    NavGpsUdp();

    bool Init();
    void Close();
    std::string Info();
    bool getSensorValues(navsens::Sensors& sensor); // return true if new data available

protected:
    QUdpSocket* udpSocket{};
    int nMsg{};   // Anzahl messages empfangen
    QByteArray simulationData;

protected slots:
    void readData();
    void processDatagram(QByteArray& datagram);
};


extern ZFNAV_API NavGpsUdp navGPSUdp;
