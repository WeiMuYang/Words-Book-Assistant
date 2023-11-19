#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent,QApplication* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    app->installNativeEventFilter(winEventFilter_ = new WinEventFilter);
    connect(winEventFilter_,&WinEventFilter::sendWords,this, &MainWindow::getWordsSlot);

    operateFile_ = new OperateFile(this);
    netWorkAccess_ = new NetworkAccess(this);
//    netWorkAccess_->Access();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getWordsSlot(WordsType status, QString words)
{
    switch (status) {
    case IsWord:
        if(operateFile_->appendWord(words)) {
            qDebug() << "append Word succeed ";
        }else{
            qDebug() << "append Word fauiled ";
        }

        break;
    case IsSentence:
        if(operateFile_->appendSentence(words)) {
            qDebug() << "append Word succeed ";
        }else{
            qDebug() << "append Word fauiled ";
        }
        break;
    default:
        break;
    }
}

