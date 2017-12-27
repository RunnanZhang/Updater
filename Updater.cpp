#include "updater.h"
#include "ui_Updater.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

Updater::Updater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Updater)
{
    ui->setupUi(this);

    _downloadDir = qApp->applicationDirPath() + "/Temp";

    ui->installBtn->setEnabled(false);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

    connect(&_networdManager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));

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
    QDir dir(_downloadDir);

    if(!dir.exists())
    {
        dir.mkpath(_downloadDir);
    }

    _downloadTime.start();

    for(auto i : _fileList)
    {
         QNetworkReply *reply = _networdManager.get(QNetworkRequest(QUrl(i)));

         _currentDownloads << reply;

        connect(reply, &QNetworkReply::downloadProgress, this, &Updater::networkReplyProgress);
    }

    ui->stackedWidget->setCurrentWidget(ui->downloadPage);
}

void Updater::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    QFileInfo info(url.path());
    QFile file(_downloadDir + QDir::separator() + info.fileName());
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();

    _currentDownloads.removeAll(reply);
    reply->deleteLater();

    // all downloads finished
    if (_currentDownloads.isEmpty())
    {
        ui->installBtn->setEnabled(true);
    }
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
}

void Updater::on_installBtn_clicked()
{

}

void Updater::setUpdateInfo(const QString &info)
{
    ui->updateInfo->setText(info);
}

#ifndef QT_NO_SSL
void Updater::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    ui->stateLabel->setText(errorString);
}
#endif
