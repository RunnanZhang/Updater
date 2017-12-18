#include "Updater.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir::setCurrent(a.applicationDirPath());

    QStringList arguments = a.arguments();
    // remove the first argument, which is the program's name
    arguments.takeFirst();

    QStringList list;
    list << "http://qt90.com/download/MagicAssistant.rar";
    Updater w;
    w.setFileList(list);
    w.show();

    return a.exec();
}
