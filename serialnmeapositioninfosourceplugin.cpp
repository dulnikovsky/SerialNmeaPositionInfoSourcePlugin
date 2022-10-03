#include "serialnmeapositioninfosourceplugin.h"

#include "extendednmeapositioninfosource.h"

#include <QSerialPortInfo>
#include <QSerialPort>

SerialNmeaPositionInfoSourcePlugin::SerialNmeaPositionInfoSourcePlugin()
{
    QList<QSerialPortInfo> serialportInfoList = QSerialPortInfo::availablePorts();
    QStringList serialPortNames;
    for(int i=0; i<serialportInfoList.size(); i++)
    {
        if(serialportInfoList.at(i).isBusy())
            continue;

        serialPort = new QSerialPort(serialportInfoList.at(i).portName());
        serialPort->setBaudRate( 4800);
        if(serialPort->open( QIODevice::ReadOnly))
        {
            serialPort->waitForReadyRead(2000);
            char buf[3];
            if( serialPort->peek( buf, 3) >= 3)
            {
                if( buf[0] == '$' && buf[1] == 'G' && buf[2] == 'P')
                {
                    break;
                }
                else
                {
                    serialPort->close();
                    serialPort->deleteLater();
                    serialPort = nullptr;
                    continue;
                }
            }
        }
        else
        {
            serialPort->deleteLater();
            serialPort = nullptr;
        }
    }

}

SerialNmeaPositionInfoSourcePlugin::~SerialNmeaPositionInfoSourcePlugin()
{
    serialPort->deleteLater();
}

QGeoPositionInfoSource *SerialNmeaPositionInfoSourcePlugin::positionInfoSource(QObject *parent)
{
    ExtendedNmeaPositionInfoSource *exNmeaPosSource = new ExtendedNmeaPositionInfoSource(QNmeaPositionInfoSource::RealTimeMode, parent);
    if(serialPort)
        exNmeaPosSource->setDevice(serialPort);
    return exNmeaPosSource;
}

QGeoSatelliteInfoSource *SerialNmeaPositionInfoSourcePlugin::satelliteInfoSource(QObject *parent)
{
    Q_UNUSED(parent)

    return nullptr;
}

QGeoAreaMonitorSource *SerialNmeaPositionInfoSourcePlugin::areaMonitor(QObject *parent)
{
    Q_UNUSED(parent)

    return nullptr;
}
