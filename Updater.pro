QT += network widgets xml

TARGET = Updater
TEMPLATE = app

SOURCES += \
        main.cpp \
        Updater.cpp \
        Settings.cpp \

HEADERS += \
        Updater.h \
        Settings.h

FORMS += \
        Updater.ui

RESOURCES += \
    Updater.qrc
