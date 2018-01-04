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

    Updater w;

    if(!arguments.isEmpty())
    {
        // get the second argument, which is the current version.
        QString version = arguments.takeFirst();

        // get the third argument, which is the update info.
        QString updateInfo = arguments.takeFirst();

        w.setFileList(arguments);
        w.setUpdateInfo(updateInfo);
        w.setVersion(version);
    }

    w.show();
    // 因为有一个隐藏的label，直接隐藏了，导致整体显示不是窗体最小值，看着不好看，临时解决.
    w.resize(w.width(), w.height() - 24);

    return a.exec();
}
