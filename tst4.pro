QT =  core network xml

win32
{
    win32-g++:contains(QMAKE_HOST.arch, x86_64):
    {
    #LIBS += -L"C:/Program Files/OpenSSL-Win64/lib" -lubsec
    INCLUDEPATH += "C:/Program Files/OpenSSL-Win64/include"
    }
}

CONFIG += c++17 cmdline console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Id.h \
    Stop.h
