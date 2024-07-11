#ifndef ID_H
#define ID_H
class Id
{
public:
    QString m_key, m_name;
    void setId(const QDomElement& root)
    {
        m_key = root.firstChildElement("key").text();
        m_name = root.firstChildElement("name").text();
    }
    virtual void dPrint() const
    {
        qDebug() << "\tKey:" << m_key;
        qDebug() << "\tName:" << m_name;
    }
    virtual ~Id() = default;
};
#endif // ID_H
