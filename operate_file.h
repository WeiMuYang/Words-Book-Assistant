#ifndef OPERATEFILE_H
#define OPERATEFILE_H

#include <QObject>
#include "data_type.h"
class OperateFile : public QObject
{
    Q_OBJECT
public:
    explicit OperateFile(QObject *parent = nullptr);

public slots:
    bool appendWord(WordInfo wordInfo);
    bool appendSentence(WordInfo sentence);

signals:

private:
    QString wordPath_;
    QString website_;
    QString audio_;

};

#endif // OPERATEFILE_H
