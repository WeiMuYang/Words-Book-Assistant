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
    void setTyporaPath(QString typoraPath);
    void setVscodePath(QString vscodePath);
public slots:
    void OpenMarkdownAndDirSlot(QString fileName);
    void CompareFileSlot(QString fileNameA, QString fileNameB);
    void OpenJsonAndIniSlot(QString fileName);
    void OpenDirSlot(QString dirName);

signals:
    void sigOpenExProLog(QString log);
private:
    QString typoraPath_;
    QString vscodePath_;
};

#endif // OPENEXPROGRAM_H
