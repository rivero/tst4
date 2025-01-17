#ifndef VARIANT_H
#define VARIANT_H
#include <unordered_map>
#include "Id.h"
extern const QString API_KEY;
class Variant : public Id
{
    QString m_destinations_xlink_ref;
public:
    Variant() = default;
    Variant(const QDomElement& root)
    {
        setId(root);
        m_destinations_xlink_ref = root.firstChildElement("destinations").attribute("xlink:href");
    }

    virtual void dPrint() const
    {
        qDebug() << "Variant:";
        Id::dPrint();
        qDebug() << "\tdestinations xlink:ref:" << m_destinations_xlink_ref;
    }
    void printStopHttpRequest() const
    {
        QString stopsPath("https://api.winnipegtransit.com/v3/stops?variant=");
        QString path = stopsPath + m_key + QString("&") + API_KEY;
        qInfo() << path;
    }
};

class Variants
{
    std::unordered_map<QString, Variant> m_map;
public:
    Variants() = default;
    Variants(const QDomElement& root)
    {
        QDomNodeList variants = root.elementsByTagName("variant");
        for (int i = 0; i < variants.count(); ++i)
        {
            QDomElement variant = variants.at(i).toElement();
            Variant var(variant);
            m_map[var.m_key] = var;

//            QString xlinkHref = variant.attribute("xlink:href");
//            qDebug() << "Variant" << i + 1 << "xlink:href:" << xlinkHref;
        }
    }
    virtual void dPrint() const
    {
        for( auto& [key, variant]: m_map)
        {
            variant.dPrint();
        }
    }
    void printStopHttpRequests() const
    {
        for( auto& [key, variant]: m_map)
        {
            variant.printStopHttpRequest();
        }

    }
};

#endif // VARIANT_H
