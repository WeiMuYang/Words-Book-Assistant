#include "open_ex_program.h"
#include <QDesktopServices>
#include <QUrl>

OpenExProgram::OpenExProgram()
{
}

void OpenExProgram::setTyporaPath(QString typoraPath)
{
    typoraPath_ = typoraPath;
}

void OpenExProgram::setVscodePath(QString vscodePath)
{
    vscodePath_ = vscodePath;
}

void OpenExProgram::OpenMarkdownAndDirSlot(QString fileName){
    QProcess* pProcess = new QProcess;
    if(fileName.size() > 3 && fileName.right(3) == ".md"){
        if(!typoraPath_.isEmpty()){
            pProcess->start(typoraPath_, QStringList(fileName));
        }
    }else{
        if(!vscodePath_.isEmpty()){
            pProcess->start(vscodePath_, QStringList(fileName));
        }
    }
}

void OpenExProgram::CompareFileSlot(QString fileNameA, QString fileNameB){
    QProcess* pProcess = new QProcess;
    if(!vscodePath_.isEmpty()){
        QStringList list;
        list.append("--diff");
        list.append(fileNameA);
        list.append(fileNameB);
        pProcess->start(vscodePath_, list);
        pProcess->waitForFinished();
    }

}

void OpenExProgram::OpenJsonAndIniSlot(QString fileName){
    QProcess* pProcess = new QProcess;
    if(!vscodePath_.isEmpty()){
        pProcess->start(vscodePath_, QStringList(fileName));
        pProcess->waitForFinished();
    }
}

void OpenExProgram::OpenDirSlot(QString dirName)
{
    dirName = QString("file:") +dirName;
    QDesktopServices::openUrl(QUrl(dirName, QUrl::TolerantMode));
}
