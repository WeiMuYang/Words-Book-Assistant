#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QObject>
#include <QNetworkAccessManager>

class NetworkAccess : public QObject
{
    Q_OBJECT
public:
    explicit NetworkAccess(QObject *parent = nullptr);

public slots:
    void Access();
    void replyFinished();

signals:

private:
    QNetworkAccessManager *manager_;
    // 创建一个 GET 请求
    QNetworkRequest request_;
    // 发送请求并获取响应
    QNetworkReply *reply_;
};

#endif // NETWORKACCESS_H
