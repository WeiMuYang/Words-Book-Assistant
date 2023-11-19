#ifndef OPERATEFILE_H
#define OPERATEFILE_H

#include <QObject>
#include "win_event_filter.h"
class OperateFile : public QObject
{
    Q_OBJECT
public:
    explicit OperateFile(QObject *parent = nullptr);

public slots:
    bool appendWord(QString words);
    bool appendSentence(QString sentence);

signals:

private:
    QString wordPath_;
    QString website_;
    QString audio_;

};

#endif // OPERATEFILE_H
