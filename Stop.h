#ifndef STOP_H
#define STOP_H
#include <unordered_map>
#include "Id.h"

class Abbr
{
public:
    QString m_abbr;
    virtual void dPrint() const
    {
        qDebug() << "\tAbbr: " << m_abbr;
    }

};


class Street : public Id, public Abbr
{
public:
    QString m_type;
    void setStreet(const QDomElement& streetElement)
    {
        setId(streetElement);
        m_type = streetElement.firstChildElement("type").text();
        m_abbr = streetElement.firstChildElement("type").attribute("abbr");
    }
    virtual ~Street()  = default;
    virtual void dPrint() const
    {
        qDebug() << "Street:";
        Abbr::dPrint();
        Id::dPrint();
    }
};

class CrossStreet : public Street
{
public:
    void setCrossStreet(const QDomElement& CrossStreet)
    {
        setStreet(CrossStreet);
    }
    virtual void dPrint() const
    {
        qDebug() << "\nCross ";
        Street::dPrint();
    }
};

struct Location
{
    QString m_lat, m_lon, m_xStr, m_yStr;
    virtual void printLocation() const
    {
        qDebug() << "\t" << m_xStr << m_lat;
        qDebug() << "\t" << m_yStr << m_lon;
    }
    void setLocation(const QDomElement& geographicElement, const QString& x, const QString& y)
    {
        m_xStr = x;
        m_yStr = y;
        m_lat = geographicElement.firstChildElement(m_xStr).text();
        m_lon = geographicElement.firstChildElement(m_yStr).text();

    }
    double x() const
    {
        return std::stod(m_lat.toStdString());
    }
    double y() const
    {
        return std::stod(m_lon.toStdString());
    }
};

class Geo : public Location
{
public:
    void setGeo(const QDomElement& geographicElement)
    {
        Location::setLocation(geographicElement, "latitude", "longitude");

    }
    virtual void dPrint() const
    {
        qDebug() << "Geo:";
        printLocation();
    }

};

class Utm : public Location
{
public:
    void setGeo(const QDomElement& geographicElement)
    {
        Location::setLocation(geographicElement, "x", "y");
    }
    virtual void dPrint() const
    {
        qDebug() << "Utm:";
        printLocation();
    }
    // Calculate planar distance
    static double planarDistance(double E1, double N1, double E2, double N2)
    {
        double dE = E2 - E1;
        double dN = N2 - N1;
        return std::sqrt(dE * dE + dN * dN);
    }
    static double planarDistance(const Utm& u1, const Utm& u2)
    {
        return planarDistance(u1.x(), u1.y(), u2.x(), u2.y());
    }

};

class Stop : public Id, public Abbr
{
    Street m_street;
    CrossStreet m_crossStreet;
public:
    Geo m_geo;
    Utm m_utm;

    Stop() = default;
    QString m_number, m_direction, m_side, m_sideAbbr;
    Stop(const QDomElement& root)
    {
        setId(root);
        m_number = root.firstChildElement("number").text();
        m_direction = root.firstChildElement("direction").text();
        m_abbr = root.firstChildElement("direction").attribute("abbr");
        m_side = root.firstChildElement("side").text();
        m_sideAbbr = root.firstChildElement("side").attribute("abbr");

        // Extract street information
        QDomElement streetElement = root.firstChildElement("street");

        // Extract cross-street information
        QDomElement crossStreetElement = root.firstChildElement("cross-street");

        // Extract geographic coordinates
        QDomElement geographicElement = root.firstChildElement("centre").firstChildElement("geographic");
        QDomElement utmElement = root.firstChildElement("centre").firstChildElement("utm");


        m_street.setStreet(streetElement);
        m_crossStreet.setCrossStreet(crossStreetElement);
        m_geo.setGeo(geographicElement);
        m_utm.setGeo(utmElement);


    }
    virtual void dPrint() const
    {
        qDebug() << "Stop:";
        Id::dPrint();
        qDebug() << "\tNumber:" << m_number;
        qDebug() << "\tDirection:" << m_direction;
        Abbr::dPrint();
        qDebug() << "\tSide:" << m_side;
        qDebug() << "\tAbbr:" << m_sideAbbr;
        m_street.dPrint();
        m_crossStreet.dPrint();
        m_geo.dPrint();
        m_utm.dPrint();

    }
};

struct MidPoint
{
    Stop m_stop;
    std::uint64_t m_distance{};
};

class Stops
{
    std::map<int, Stop> m_map;
    std::uint64_t m_distance{};
    MidPoint m_midPoint;
public:
    Stops() = default;
    /*
        Calculate the following
        > Find the longest Variant by total travelling distance from first to last stop.

        Assume the first stop is
        connected to the second(crow flies), and the second to the 3rd, and so on, for path calculation.
        For example, list of stops for Variant 16-1-L returned by query "stops?variant=16-1-L" is the order in which
        the bus visits them and assume that there is a straight-line path between each set of consecutive stops.
        * Output the result to console with Variant key, name, and length in meters in descending order by
        length.
    */
    Stops(const QDomElement& root)
    {
        QDomNodeList stops = root.elementsByTagName("stop");
        Utm prevUtm;
        for (int i = 0; i < stops.count(); ++i)
        {
            QDomElement stop = stops.at(i).toElement();
            Stop st(stop);
            m_map[st.m_number.toInt()] = st;
            if(prevUtm.m_lat.isEmpty())
            {
                prevUtm = st.m_utm;
                continue;
            }
            Utm thisUtm = st.m_utm;
            m_distance += Utm::planarDistance(prevUtm, thisUtm); // total distance is calculated here
            prevUtm = thisUtm;
        }
    }
    virtual void dPrint() const
    {
        for( auto& [key, stop]: m_map)
        {
            qInfo() << "Printing key:" << key;
            stop.dPrint();
        }
    }
    std::uint64_t calcDistance()
    {
        return m_distance;
    }
    /*
        > Find the mid-way/half-distance point coordinates on the longest Variant by length.
        * Output the Variant key, name, total length(meters), and the mid-point coordinates to console.
    */
    MidPoint calcMidPoint()
    {
        // iterate the stop map and find where the distance calculated is about mid point meaning
        // the previous distance is less and the next distance is grater.
        // Store
        auto midDistance = m_distance/2;
        std::uint64_t localDistance{};
        Stop prevStop;
        for (auto& [number, stop] : m_map)
        {
            if(prevStop.m_number.isEmpty())
            {
                prevStop = stop;
                continue;
            }
            localDistance += Utm::planarDistance(prevStop.m_utm, stop.m_utm);
            if(localDistance > midDistance )
            {
                m_midPoint.m_stop = prevStop;
                m_midPoint.m_distance = localDistance;
                break;
            }
        }
        return m_midPoint;
    }
};

#endif // STOP_H
