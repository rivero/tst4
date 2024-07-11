#include <QCoreApplication>
#include <QtNetwork>
#include <QtCore>
#include <QDomDocument>
#include <QDomElement>
#include <Variant.h>
#include <Stop.h>
#include <filereader.h>
#include <unordered_map>
using namespace std;
const QString API_KEY("api-key=hDk1zowO1Y0FTNOXy3Ut");

namespace data
{
    // This will get me all the data I need to download the stops.
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
}

namespace readers
{
    template <typename T, typename U>
    void printHttp()
    {
        for (auto& [key, value] : data::variantsMap<T,U> )
        {
            value.printStopHttpRequests(); // To download the stops
        }
    }
    template <typename T, typename U>
    void print()
    {
        for (auto& [key, value] : data::variantsMap<T,U> )
        {
            value.dPrint(); // To download the stops
        }
    }
    void readVariants()
    {
        data::readFile<int, Variants>(16, "C:/tools/repos/tst4/xml/variants_16.xml");
        printHttp<int, Variants>();
    }
    void readStops()
    {
        data::readFile<QString, Stops>("16-0-ASTERISK", "C:/tools/repos/tst4/xml/stops_16-0-ASTERISK.xml");
        print<QString, Stops>();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    readers::readVariants();
    readers::readStops();
    return a.exec();
}
