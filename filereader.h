#ifndef FILEREADER_H
#define FILEREADER_H

class FileReader
{
    QByteArray m_bytes;
    QDomElement m_root;

public:
    FileReader(const QString& fileName);
    QByteArray data() const;
    bool parseXML();
    QDomElement getRoot() const;
};

#endif // FILEREADER_H
