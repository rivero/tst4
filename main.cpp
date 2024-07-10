#include <QCoreApplication>
#include <QtNetwork>
#include <QtCore>
#include <QDomDocument>
#include <QDomElement>
#include <Stop.h>

using namespace std;

void parseXML(const QByteArray& xmlContent)
{
    QDomDocument doc;
    if (doc.setContent(xmlContent))
    {
        // Retrieve the root element
        QDomElement root = doc.documentElement();
        Stop stop(root);
        stop.dPrint();

    } else {
        qDebug() << "Error parsing XML.";
    }
}

class RESTClient : public QObject
{
public:
    void get()
    {
        QNetworkAccessManager Manager;
        QUrl url("https://api.winnipegtransit.com/v3/stops/10064?api-key=hDk1zowO1Y0FTNOXy3Ut");
        QNetworkRequest request(url);
        QNetworkReply *reply = Manager.get(request);
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "REST get Error " << reply->errorString();
        }
        QByteArray ba = reply->readAll();
        parseXML(ba);
    }
};
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //QString stop_response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<stop xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"stops/10064\" xlink:base=\"\" query-time=\"2024-07-10T11:14:31\">\n  <key>10064</key>\n  <name>Northbound Osborne at Glasgow</name>\n  <number>10064</number>\n  <direction abbr=\"NB\">Northbound</direction>\n  <side abbr=\"NS\">Nearside</side>\n  <street xlink:href=\"streets/2715\">\n    <key>2715</key>\n    <name>Osborne Street</name>\n    <type abbr=\"St\">Street</type>\n  </street>\n  <cross-street xlink:href=\"streets/1486\">\n    <key>1486</key>\n    <name>Glasgow Avenue</name>\n    <type abbr=\"Ave\">Avenue</type>\n  </cross-street>\n  <centre xmlns:utm=\"http://winnipegtransit.com/coordinates/utm\" xmlns:ang=\"http://winnipegtransit.com/coordinates/angular\">\n    <utm zone=\"14U\">\n      <x>633838</x>\n      <y>5525742</y>\n    </utm>\n    <geographic>\n      <latitude>49.86912</latitude>\n      <longitude>-97.1375</longitude>\n    </geographic>\n  </centre>\n</stop>\n";

    //parseXML(stop_response);
    RESTClient rest;
    rest.get();

    return a.exec();
}
