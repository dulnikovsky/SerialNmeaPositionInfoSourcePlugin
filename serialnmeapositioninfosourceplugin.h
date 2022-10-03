#ifndef SERIALNMEAPOSITIONINFOSOURCEPLUGIN_H
#define SERIALNMEAPOSITIONINFOSOURCEPLUGIN_H

#include <QGeoPositionInfoSourceFactory>

class ExtendedNmeaPositionInfoSource;
class QSerialPort;

class SerialNmeaPositionInfoSourcePlugin : public QObject, public QGeoPositionInfoSourceFactory
{
    Q_OBJECT


    Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/5.0"
                      FILE "plugin.json")


    Q_INTERFACES(QGeoPositionInfoSourceFactory)

public:
    SerialNmeaPositionInfoSourcePlugin();

    ~SerialNmeaPositionInfoSourcePlugin() override;

    QGeoPositionInfoSource *positionInfoSource(QObject *parent) override;
    QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent) override;
    QGeoAreaMonitorSource *areaMonitor(QObject *parent) override;

private:

    QSerialPort *serialPort{nullptr};
};

#endif // SERIALNMEAPOSITIONINFOSOURCEPLUGIN_H
