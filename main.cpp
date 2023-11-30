#include "mainwindow.h"

#include <QApplication>
#include <QFont>
#include <QStandardPaths>
#include <QDesktopWidget>
#include <QRect>
#include <QString>

int screenInfo()
{
    QDesktopWidget *dwsktopwidget = QApplication::desktop();
    QRect deskrect = dwsktopwidget->availableGeometry();
    QRect screenrect = dwsktopwidget->screenGeometry();
    int scrcount = dwsktopwidget->screenCount();
    qCritical("screenrect.w==%s\n",qPrintable(QString::number(screenrect.width())));
    qCritical("screenrect.h==%s\n",qPrintable(QString::number(screenrect.height())));
    qCritical("deskrect.w==%s\n",qPrintable(QString::number(deskrect.width())));
    qCritical("deskrect.h==%s\n",qPrintable(QString::number(deskrect.height())));
    qCritical("scrcount==%s\n",qPrintable(QString::number(scrcount)));
    return deskrect.width();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(nullptr, &a);;
    int width = screenInfo();
    if(width >= 3840){
        QFont font = a.font();
        font.setPointSize(13);
        a.setFont(font);
    }else{
        QFont font = a.font();
        font.setPointSize(10);
        a.setFont(font);
    }
    w.show();
    return a.exec();
}
