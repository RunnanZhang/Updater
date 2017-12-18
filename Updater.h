#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include <QNetworkAccessManager>

namespace Ui {
class Updater;
}

class QFile;

class Updater : public QWidget
{
    Q_OBJECT

public:
    explicit Updater(QWidget *parent = 0);
    ~Updater();

private slots:
    void on_downloadBtn_clicked();
    void on_installBtn_clicked();

#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
#endif

private:
    Ui::Updater *ui;

    QNetworkAccessManager _networdManager;

    QNetworkReply *_reply;

    QFile *_file;
};

#endif // UPDATER_H
