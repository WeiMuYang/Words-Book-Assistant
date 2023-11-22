#ifndef OPENEXPROGRAM_H
#define OPENEXPROGRAM_H

#include <QObject>
#include <QMap>
#include <QProcess>

class OpenExProgram: public QObject
{
    Q_OBJECT
public:
    OpenExProgram();
    void setSoftWarePath(QMap<QString,QString> softWarePath);
    QString getPathByKey(QString key);
public slots:
    void OpenMarkdownAndDirSlot(QString fileName);
    void CompareFileSlot(QString fileNameA, QString fileNameB);
    void OpenJsonAndIniSlot(QString fileName);
    void OpenDirSlot(QString dirName);

signals:
    void sigOpenExProLog(QString log);
private:

    QMap<QString,QString> softWarePathMap_;
};

#endif // OPENEXPROGRAM_H
