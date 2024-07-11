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
            //variants.dPrint();
            variants.printStopHttpRequests(); // To download the stops
        }
    }
}

namespace variants
{
    void readVariants()
    {
        data::readFile<int, Variants>(16, "C:/tools/repos/tst4/xml/variants_16.xml");
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    variants::readVariants();
    return a.exec();
}
