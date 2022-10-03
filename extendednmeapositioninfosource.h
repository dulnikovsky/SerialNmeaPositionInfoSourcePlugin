/****************************************************************************

Copyright (C) 2014 Robert Vetter
Contact: http://www.robertvetter.com

****************************************************************************/

#ifndef EXTENDEDNMEAPOSITIONINFOSOURCE_H
#define EXTENDEDNMEAPOSITIONINFOSOURCE_H

#include <QNmeaPositionInfoSource>
#include <QGeoSatelliteInfo>

class ExtendedNmeaPositionInfoSource : public QNmeaPositionInfoSource
{
    Q_OBJECT
public:
    explicit ExtendedNmeaPositionInfoSource(UpdateMode updateMode,QObject *parent = nullptr);

signals:
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> & satellites);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> & satellites);


protected:
    bool parsePosInfoFromNmeaData(const char * data, int size, QGeoPositionInfo * posInfo, bool * hasFix) override;

private:
    QDateTime lastGGA;
    QDateTime lastRMC;

    qreal lastSpeed;
    qreal lastHeading;
    qreal lastAltitude;
    qreal lastHDilution;
    qreal lastVDilution;

    QList<QGeoSatelliteInfo> satsInViewList;
};

#endif // EXTENDEDNMEAPOSITIONINFOSOURCE_H
