#include "updater.h"
#include "ui_Updater.h"
#include "Settings.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

Updater::Updater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Updater)
{
    ui->setupUi(this);

    _downloadDir = qApp->applicationDirPath() + "/Temp";

    ui->stateLabel->hide();

    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

    connect(&_networdManager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
    connect(ui->completeBtn, &QPushButton::clicked, this, &Updater::close);

#ifndef QT_NO_SSL
    connect(&_networdManager, &QNetworkAccessManager::sslErrors, this, &Updater::sslErrors);
#endif
}

Updater::~Updater()
{
    delete ui;
}

void Updater::setFileList(const QStringList &list)
{
    _fileList = list;
}

void Updater::on_downloadBtn_clicked()
{
    _downloadTime.start();

    for(auto i : _fileList)
    {
         QNetworkReply *reply = _networdManager.get(QNetworkRequest(QUrl(i)));

         _currentDownloads << reply;

        connect(reply, &QNetworkReply::downloadProgress, this, &Updater::networkReplyProgress);
    }

    ui->stackedWidget->setCurrentWidget(ui->downloadPage);

    // 开启loading.
    QMovie *movie = new QMovie(":/images/loading.gif");
    ui->loadLabel->setMovie(movie);
    movie->start();
}

void Updater::downloadFinished(QNetworkReply *reply)
{
    // 下载完毕后再写到本地，先备份当前程序至Temp文件夹下，在将新程序写入当前文件夹下.
    QString tempDir = qApp->applicationDirPath() + "/Temp";
    QDir dir(tempDir);
    if(!dir.exists()) {
        dir.mkpath(tempDir);
    }

    QUrl url = reply->url();
    QFileInfo info(url.path());
    QString filePath = qApp->applicationDirPath() + QDir::separator() + info.fileName();
    QString tempFilePath = tempDir + QDir::separator() + info.fileName();

    if (QFile::exists(filePath)) {
        if(QFile::exists(tempFilePath)) {
            QFile::remove(tempFilePath);
        }
        QFile::copy(filePath, tempFilePath);
        QFile::remove(filePath);
    }

    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();

    _currentDownloads.removeAll(reply);
    reply->deleteLater();

    // all downloads finished
    if (_currentDownloads.isEmpty())
    {
        // 更新版本号.
        Settings setting("./version.xml");
        setting.setValue("version", _version);

        ui->completeBtn->setEnabled(true);
    }
}

void Updater::on_completeBtn_clicked()
{
    QProcess::startDetached("MagicAssistant.exe");
    qApp->quit();
}

void Updater::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);

    // calculate the download speed
    double speed = bytesRead * 1000.0 / _downloadTime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    QString s = QString("%1 %2").arg(speed, 0, 'f', 2).arg(unit);
    double read = bytesRead;
    double total = totalBytes;

    if (total < 1024) {
        unit = "bytes";
    } else if (total < 1024*1024) {
        total /= 1024;
        read /= 1024;
        unit = "kB";
    } else {
        total /= 1024*1024;
        read /= 1024*1024;
        unit = "MB";
    }

    s += QString(", %3/%4 %5").arg(read, 0, 'f', 2).arg(total, 0, 'f', 2).arg(unit);
    ui->progressLabel->setText(s);
    //ui->progressBar->setFormat("%p% " + s);
}

void Updater::setUpdateInfo(const QString &info)
{
    ui->updateInfo->setText(info);
}

void Updater::setVersion(const QString &version)
{
    _version = version;
}

#ifndef QT_NO_SSL
void Updater::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    ui->stateLabel->show();
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    ui->stateLabel->setText(errorString);
}
#endif
