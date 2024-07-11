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
    Geo m_geo;
public:
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

class Stops
{
    std::map<int, Stop> m_map;
    std::vector<Utm> m_utms;
    std::uint64_t m_distance{};
public:
    Stops() = default;
    Stops(const QDomElement& root)
    {
        QDomNodeList stops = root.elementsByTagName("stop");
        for (int i = 0; i < stops.count(); ++i)
        {
            QDomElement stop = stops.at(i).toElement();
            Stop st(stop);
            m_map[std::stoi(st.m_number.toStdString())] = st;
            m_utms.push_back(st.m_utm);
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
        auto prevUtm = m_utms[0];
        for(size_t i = 1; i < m_utms.size(); i++)
        {
            auto thisUtm = m_utms[i];
            m_distance += Utm::planarDistance(prevUtm, thisUtm);
            prevUtm = thisUtm;
        }
        return m_distance;

    }
};

#endif // STOP_H
