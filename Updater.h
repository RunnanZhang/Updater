#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QTime>

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

    void setFileList(const QStringList &list);

    void setUpdateInfo(const QString &info);

    void setVersion(const QString &version);

private slots:
    void on_downloadBtn_clicked();

    void downloadFinished(QNetworkReply *reply);

    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

#ifndef QT_NO_SSL
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
#endif

private:
    Ui::Updater *ui;

    QNetworkAccessManager _networdManager;

    QList<QNetworkReply*> _currentDownloads;

    QStringList _fileList;

    QString _downloadDir;

    QTime _downloadTime;

    QString _version;
};

#endif // UPDATER_H
