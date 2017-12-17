#include "updater.h"
#include "ui_updater.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

Updater::Updater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Updater)
{
    ui->setupUi(this);

#ifndef QT_NO_SSL
    connect(&_networdManager, &QNetworkAccessManager::sslErrors,
            this, &Updater::sslErrors);
#endif
}

Updater::~Updater()
{
    delete ui;
}

void Updater::on_downloadBtn_clicked()
{
    _file = new QFile("D:/ScreenSaver.rar");
    _file->open(QIODevice::WriteOnly);

    _reply = _networdManager.get(QNetworkRequest(QUrl("http://qt90.com/download/ScreenSaver.rar")));//ScreenSaver.rar
    connect(_reply, &QNetworkReply::finished, this, [=]() {
            _file->close();
            delete _file;
            _file = Q_NULLPTR;
    });
    connect(_reply, &QIODevice::readyRead, this, [=](){
        if (_file) {
            _file->write(_reply->readAll());
            qDebug() << 123;
        }
    });
}

void Updater::on_installBtn_clicked()
{

}

void Updater::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
{

}
