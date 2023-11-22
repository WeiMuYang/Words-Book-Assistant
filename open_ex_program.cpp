#include "open_ex_program.h"
#include <QDesktopServices>
#include <QUrl>

OpenExProgram::OpenExProgram()
{

}

void OpenExProgram::setSoftWarePath(QMap<QString, QString> softWarePath)
{
    softWarePathMap_.swap(softWarePath);
}

QString OpenExProgram::getPathByKey(QString key){
    auto it = softWarePathMap_.find(key);
    if(it == softWarePathMap_.end()){
        emit sigOpenExProLog("Can't find SoftWare: " + key + "'s Path!!!");
        return QString("");
    }else{
        return it.value();
    }
}

void OpenExProgram::OpenMarkdownAndDirSlot(QString fileName){
    QProcess* pProcess = new QProcess;
    if(fileName.size() > 3 && fileName.right(3) == ".md"){
        QString pathTypora = getPathByKey("Typora");
        if(!pathTypora.isEmpty()){
            pProcess->start(pathTypora, QStringList(fileName));
        }
    }else{
        QString pathVsCode = getPathByKey("Vscode");
        if(!pathVsCode.isEmpty()){
            pProcess->start(pathVsCode, QStringList(fileName));
        }
    }
}

void OpenExProgram::CompareFileSlot(QString fileNameA, QString fileNameB){
    QProcess* pProcess = new QProcess;
    QString pathVsCode = getPathByKey("Vscode");
    if(!pathVsCode.isEmpty()){
        QStringList list;
        list.append("--diff");
        list.append(fileNameA);
        list.append(fileNameB);
        pProcess->start(pathVsCode, list);
        pProcess->waitForFinished();
    }

}

void OpenExProgram::OpenJsonAndIniSlot(QString fileName){
    QProcess* pProcess = new QProcess;
    QString pathVsCode = getPathByKey("Vscode");
    if(!pathVsCode.isEmpty()){
        pProcess->start(pathVsCode, QStringList(fileName));
        pProcess->waitForFinished();
    }
}

void OpenExProgram::OpenDirSlot(QString dirName)
{
    dirName = QString("file:") +dirName;
    QDesktopServices::openUrl(QUrl(dirName, QUrl::TolerantMode));
}
