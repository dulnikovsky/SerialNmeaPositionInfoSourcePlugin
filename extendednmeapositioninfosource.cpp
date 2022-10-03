
#include "extendednmeapositioninfosource.h"

static bool hasValidNmeaChecksum(const char *data, int size)
{
    int asteriskIndex = -1;
    for (int i = 0; i < size; ++i) {
        if (data[i] == '*') {
            asteriskIndex = i;
            break;
        }
    }

    const int CSUM_LEN = 2;
    if (asteriskIndex < 0 || asteriskIndex + CSUM_LEN >= size)
        return false;

    // XOR byte value of all characters between '$' and '*'
    int result = 0;
    for (int i = 1; i < asteriskIndex; ++i)
        result ^= data[i];

    bool tmpbool;
    return (result == QString(QByteArray(&data[asteriskIndex + 1], 2)).toInt(&tmpbool,16));
}

ExtendedNmeaPositionInfoSource::ExtendedNmeaPositionInfoSource(UpdateMode updateMode,QObject *parent) :
    QNmeaPositionInfoSource(updateMode, parent),lastSpeed(-1),lastHeading(-1),lastAltitude(-10000),lastHDilution(-1),lastVDilution(-1)
{

}

bool ExtendedNmeaPositionInfoSource::parsePosInfoFromNmeaData(const char * data, int size, QGeoPositionInfo * posInfo, bool * hasFix)
{

    bool ret=QNmeaPositionInfoSource::parsePosInfoFromNmeaData(data,size,posInfo,hasFix);
    if(ret==false)
    {
        if (size < 6 || data[0] != '$' || !hasValidNmeaChecksum(data, size))
            return false;

        if (data[3] == 'G' && data[4] == 'S' && data[5] == 'V')
        {
            if(satsInViewList.size()>32)
                satsInViewList.clear();

            QByteArray sentence(data, size-5);
            QList<QByteArray> parts = sentence.split(',');
            if(parts.size()>4)
            {
                int totalSentences=QString(parts.at(1)).toInt();
                int sentenceNumber=QString(parts.at(2)).toInt();
                if(sentenceNumber==1)
                    satsInViewList.clear();

                for(int i=7;i<parts.size();i+=4)
                {
                    bool ok;
                    int satID= QString(parts.at(i-3)).toInt(&ok);
                    if(! ok)
                        continue;
                    QGeoSatelliteInfo satInfo;
                    satInfo.setSatelliteIdentifier(satID );
                    satInfo.setAttribute(QGeoSatelliteInfo::Elevation,QString(parts.at(i-2)).toInt() );
                    satInfo.setAttribute(QGeoSatelliteInfo::Azimuth,QString(parts.at(i-1)).toInt() );
                    satInfo.setSignalStrength(QString(parts.at(i)).toInt(&ok) );
                    if(ok)
                        satsInViewList.append(satInfo);
                }
                if(sentenceNumber==totalSentences)
                    emit satellitesInViewUpdated(satsInViewList);
            }
        }
        else if (data[3] == 'G' && data[4] == 'S' && data[5] == 'A')
        {
            QByteArray sentence(data, size-5);
            QList<QByteArray> parts = sentence.split(',');

            if(parts.size()!=18)
                return false;

            QList<QGeoSatelliteInfo> satsInUseList;
            for(int i=3;i<15;i++)
            {
                bool ok;
                int satID = QString(parts.at(i)).toInt(&ok);
                if(!ok)
                    continue;

                for(int x=0;x<satsInViewList.size();x++)
                {
                    if(satsInViewList.at(x).satelliteIdentifier()==satID)
                        satsInUseList.append(satsInViewList.at(x));
                }
            }
            emit satellitesInUseUpdated(satsInUseList);
            bool ok;
            qreal vdilution = QString(parts.at(17)).toDouble(&ok);
            if(ok)
                lastVDilution=vdilution;
        }
        return false;
    }
    if (data[3] == 'G' && data[4] == 'G' && data[5] == 'A')
    {
        if(lastRMC.secsTo(posInfo->timestamp())<=2)
        {
            if(lastSpeed>=0)
                posInfo->setAttribute(QGeoPositionInfo::GroundSpeed,lastSpeed);

            if(lastHeading>=0)
                posInfo->setAttribute(QGeoPositionInfo::Direction,lastHeading);
        }

        QByteArray sentence(data, size);
        QList<QByteArray> parts = sentence.split(',');
        if (parts.count() > 8 && parts[8].count() > 0)
        {
            bool hasHDilution= false;
            double val = parts[8].toDouble(&hasHDilution);
            if (hasHDilution)
            {
                posInfo->setAttribute(QGeoPositionInfo::HorizontalAccuracy,val);
                lastHDilution=val;
            }
            else
                lastHDilution=-1;
        }

        if(posInfo->coordinate().type()==QGeoCoordinate::Coordinate3D)
            lastAltitude=posInfo->coordinate().altitude();
        else
            lastAltitude=-10000;

        if(lastVDilution>=0)
            posInfo->setAttribute(QGeoPositionInfo::VerticalAccuracy,lastVDilution);

        lastGGA=posInfo->timestamp();
    }
    else if (data[3] == 'R' && data[4] == 'M' && data[5] == 'C')
    {
        if(lastGGA.secsTo(posInfo->timestamp())<=2)
        {
            if(lastHDilution>=0)
                posInfo->setAttribute(QGeoPositionInfo::HorizontalAccuracy,lastHDilution);

            if(lastVDilution>=0)
                posInfo->setAttribute(QGeoPositionInfo::VerticalAccuracy,lastVDilution);

            if(lastAltitude>=-9000)
                posInfo->coordinate().setAltitude(lastAltitude);
        }


        if(posInfo->hasAttribute(QGeoPositionInfo::GroundSpeed))
            lastSpeed=posInfo->attribute(QGeoPositionInfo::GroundSpeed);
        else
            lastSpeed=-1;

        if(posInfo->hasAttribute(QGeoPositionInfo::Direction))
            lastHeading=posInfo->attribute(QGeoPositionInfo::Direction);
        else
            lastHeading=-1;

        lastRMC=posInfo->timestamp();
    }

    return ret;
}
