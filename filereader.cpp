#include <QCoreApplication>
#include <QtNetwork>
#include <QtCore>
#include <QDomDocument>
#include <QDomElement>
#include "filereader.h"

FileReader::FileReader(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        m_bytes = file.readAll();
        // Process the data
        file.close();
    }
}

bool FileReader::parseXML()
{
    if(m_bytes.size() == 0)
        return false;

    QDomDocument doc;
    if (doc.setContent(m_bytes))
    {
        m_root = doc.documentElement();
        return true;
    }
    else
    {
        qDebug() << "Error parsing XML.";
    }
    return false;
}

const QDomElement& FileReader::getRoot() const
{
    return m_root;
}

