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
#include <limits>

using namespace std;
/*
I had issues installing OpenSSL and lost most of the first day.

I had to download the relevant files manually and save them to Dwoanload_Folder.

Thanks

Jason J. Rivero
sr.jrivero@gmail.com
1.778.552.5212

*/
const QString API_KEY("api-key=hDk1zowO1Y0FTNOXy3Ut");
const QString Download_Folder("C:/tools/repos/tst4/xml/");
Box Stops::m_boundingBox;

namespace data
{
    template <typename T, typename U>
    unordered_map<T,U> variantsMap;

    template <typename T, typename U>
    void readFile(T route, const QString& file)
    {
        QString theFile;
        if(route == -1)
            theFile = Download_Folder + file + QString("_BLUE") +  ".xml";
        else
            theFile = Download_Folder + file + QString("_") +  QString::number(route) +  ".xml";

        // looks like this? "C:/tools/repos/tst4/xml/variants_16.xml"
        FileReader reader(theFile);
        if (reader.parseXML())
        {
            U variants(reader.getRoot());
            variantsMap<T,U>[route] = variants;
        }
    }

    unordered_map<QString, vector<Stops> > variantStopsMap;
    void readStopsFile(const QString& variant)
    {
        QString file = Download_Folder + QString("stops_") + variant + ".xml";
        qInfo() << "Reading stops file" << file;
        FileReader reader(file);
        if (reader.parseXML())
        {
            Stops stops(reader.getRoot());
            variantStopsMap[variant].push_back(stops);
        }
        else
            qWarning() << "Error reading/parsing file" << file;
    }

    set < pair<uint64_t, QString> > distances;
    map<QString, MidPoint> midPoints;
    void printStopsMap(bool longPrint = false)
    {
        for(auto& [variant, stopsVec]: variantStopsMap)
        {
            qDebug() << "\nVariant: " << variant;
            for(auto& stops: stopsVec)
            {
                if (longPrint)
                    stops.dPrint();
                auto dist = stops.calcDistance();
                distances.insert({dist, variant});
                qDebug() << "Total distance: " << dist;

                /*
                    > Find the mid-way/half-distance point coordinates on the longest Variant by length.
                    * Output the Variant key, name, total length(meters), and the mid-point coordinates to console.
                */
                midPoints[variant] = stops.calcMidPoint();
            }
        }
        // Based on distance calculation, largest distance is displayed here.
        // Output the result to console with Variant key, name, and length in meters in descending order by length.
        for(auto & [distance, variantName]: distances)
        {
            qInfo() << "Variant: " << variantName << "distance:" << distance;
        }
        auto [distance, variant] = *distances.rbegin();
        qInfo() << "Responses:\nCalculate the following\n \
        > Find the longest Variant by total travelling distance from first to last stop.\n\
        Assume the first stop is connected to the second(crow flies), and the second to the 3rd, and so on, for path calculation.\n\
        For example, list of stops for Variant 16-1-L returned by query \"stops?variant=16-1-L\" is the order in which\n\
        the bus visits them and assume that there is a straight-line path between each set of consecutive stops.\n\
        * Output the result to console with Variant key, name, and length in meters in descending order by length.";
        qInfo() << "\nLongest Variant: [" << variant << "] : " << distance << " meters";

        qInfo() << "\n> Find the mid-way/half-distance point coordinates on the longest Variant by length.\n\
                       * Output the Variant key, name, total length(meters), and the mid-point coordinates to console.";
        auto midPoint = midPoints[variant];
        qInfo() << "\nMid point for [" << variant << "] distance: " << midPoint.m_distance << "meters";
        midPoint.m_stop.m_geo.dPrint();

        qInfo() << "\nMaximum Distance" << Stops::maxDistance();
        auto boundingBox = Stops::boundingBox();
        qInfo() << "\nBounding Box:\nUpper left";
        boundingBox.upperLeft.printLocation();
        qInfo() << "Lower Right";
        boundingBox.bottomRight.printLocation();

    }

}

namespace readers
{
    template <typename T, typename U>
    void printStopHttpRequests()
    {
        for (auto& [route, value] : data::variantsMap<T,U> )
        {
            qInfo() << "Values for downloading for route:" << route << " please dowload them into: C:/tools/repos/tst4/xml";
            value.printStopHttpRequests(); // To download the stops
        }
    }

    // This will get me all the data I need to download the stops.
    // See: variable Download_Folder Where I stored them.
    void readVariants()
    {
        data::readFile<int, Variants>(16, "variants");
        data::readFile<int, Variants>(18, "variants");
        data::readFile<int, Variants>(33, "variants");
        data::readFile<int, Variants>(60, "variants");
        data::readFile<int, Variants>(-1, "variants");
        printStopHttpRequests<int, Variants>();
    }

    namespace StopsNames
    {
        vector<QString> stop18
            {
                "18-0-J",
                "18-0-R",
                "18-0-T",
                "18-1-A",
                "18-1-C",
                "18-1-E"
            };

        vector<QString> stop33
            {
                "33-0-J",
                "33-0-M",
                "33-1-D",
                "33-1-D2",
                "33-1-M"
            };
        vector<QString> stop60
            {
                "60-0-U",
                "60-1-D"
            };
        vector<QString> stopBLUE
            {
                "BLUE-0-S",
                "BLUE-0-U",
                "BLUE-1-D"
            };

        void loadStops(const vector<QString>& vec)
        {
            for(auto& file : vec)
            {
                data::readStopsFile(file);
            }
        }
    }

    void readStops()
    {
        data::readStopsFile("16-0-ASTERISK");
        data::readStopsFile("16-0-B");
        data::readStopsFile("16-0-K");
        data::readStopsFile("16-0-M");
        data::readStopsFile("16-1-ASTERISK");
        data::readStopsFile("16-1-HASHHASH");
        data::readStopsFile("16-1-K");
        data::readStopsFile("16-1-L");
        data::readStopsFile("16-1-P");
        data::readStopsFile("16-1-V");
        StopsNames::loadStops(StopsNames::stop18);
        StopsNames::loadStops(StopsNames::stop33);
        StopsNames::loadStops(StopsNames::stop60);
        StopsNames::loadStops(StopsNames::stopBLUE);
        data::printStopsMap(false); // no long print
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Stops::setBoundingBox();

    // readers::readVariants(); only needed first time
    readers::readStops();
    return a.exec();
}
