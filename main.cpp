#include <QCoreApplication>
#include <QtNetwork>
#include <QtCore>
#include <QDomDocument>
#include <QDomElement>
#include <Variant.h>
#include <Stop.h>
#include <filereader.h>
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;
const QString API_KEY("api-key=hDk1zowO1Y0FTNOXy3Ut");

namespace data
{
    template <typename T, typename U>
    unordered_map<T,U> variantsMap;

    template <typename T, typename U>
    void readFile(T route, const QString& file)
    {
        FileReader reader(file);
        if (reader.parseXML())
        {
            U variants(reader.getRoot());
            variantsMap<T,U>[route] = variants;
        }
    }

    unordered_map<QString, vector<Stops> > variantStopsMap;
    void readStopsFile(const QString& variant, const QString& file)
    {
        FileReader reader(file);
        if (reader.parseXML())
        {
            Stops stops(reader.getRoot());
            variantStopsMap[variant].push_back(stops);
        }
    }

    set < pair<uint64_t, QString> > distances;
    void printStopsMap()
    {
        for(auto& [variant, stopsVec]: variantStopsMap)
        {
            qDebug() << "Variant: " << variant;
            for(auto& stops: stopsVec)
            {
                stops.dPrint();
                auto dist = stops.calcDistance();
                distances.insert({dist, variant});
                qDebug() << "Total distance: " << dist;
            }
        }

        auto biggest = *distances.rbegin();
        qInfo() << "Largest distance so far: [" << biggest.second << "] : " << biggest.first << " meters";

    }

}

namespace readers
{
    template <typename T, typename U>
    void printStopHttpRequests()
    {
        for (auto& [key, value] : data::variantsMap<T,U> )
        {
            value.printStopHttpRequests(); // To download the stops
        }
    }

    // This will get me all the data I need to download the stops.
    void readVariants()
    {
        data::readFile<int, Variants>(16, "C:/tools/repos/tst4/xml/variants_16.xml");
        printStopHttpRequests<int, Variants>();
    }
    // Stops downloaded. Read, and calculate distance
    void readStops()
    {
        data::readStopsFile("16-0-ASTERISK", "C:/tools/repos/tst4/xml/stops_16-0-ASTERISK.xml");
        data::printStopsMap();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    readers::readVariants();
    readers::readStops();
    return a.exec();
}
