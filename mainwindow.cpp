#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "data_type.h"


MainWindow::MainWindow(QWidget *parent,QApplication* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    app->installNativeEventFilter(winEventFilter_ = new WinEventFilter);
    connect(winEventFilter_,&WinEventFilter::sendWords,this, &MainWindow::getWordsSlot);

    operateFile_ = new OperateFile(this);
    netWorkAccess_ = new NetworkAccess(this);
    connect(netWorkAccess_,&NetworkAccess::sendWordInfo,this, &MainWindow::appendWordInfoSlot);
    connect(netWorkAccess_,&NetworkAccess::sendSentenceInfo,this, &MainWindow::appendSentenceInfoSlot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getWordsSlot(WordsType status, QString words)
{
    switch (status) {
    case IsWord:
        netWorkAccess_->accessWord(words);
        break;
    case IsSentence:
        netWorkAccess_->accessSentence(words);
        break;
    default:
        break;
    }
}

void MainWindow::appendWordInfoSlot(WordInfo wordInfo) {

    if(operateFile_->appendWord(wordInfo)) {
        qDebug() << "append Word succeed ";
    }else{
        qDebug() << "append Word fauiled ";
    }
}

void MainWindow::appendSentenceInfoSlot(WordInfo wordInfo) {

    if(operateFile_->appendSentence(wordInfo)) {
        qDebug() << "append Sentence succeed ";
    }else{
        qDebug() << "append Sentence fauiled ";
    }
}
