#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include "data_type.h"


MainWindow::MainWindow(QWidget *parent,QApplication* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowStyle();
    app->installNativeEventFilter(winEventFilter_ = new WinEventFilter);
    connect(winEventFilter_,&WinEventFilter::sendWords,this, &MainWindow::getWordsSlot);

    operateFile_ = new FileOperation(this);

    netWorkAccess_ = new NetworkAccess(this);
    connect(netWorkAccess_,&NetworkAccess::sendWordInfo,this, &MainWindow::appendWordInfoSlot);
    connect(netWorkAccess_,&NetworkAccess::sendSentenceInfo,this, &MainWindow::appendSentenceInfoSlot);

    aboutDlg_ = new AboutDialog(this);
    openExPro_ = new OpenExProgram;
    initActions();
    initConfInfo();
}

void MainWindow::initConfInfo() {
    QString iniPath, confDirPath;
    getConfPath(iniPath, confDirPath);
    if(operateFile_->readIniFile(iniPath, iniInfo_)){
        if(!operateFile_->readUserFile(confDirPath + "/" + iniInfo_.m_HostName + ".json", userInfo_)){
            appendTextToLog("读取用户文件失败");
        }
    }else {
        appendTextToLog("读取系统文件失败");
    }
}


void MainWindow::initActions() {
    connect(ui->actionAbout,&QAction::triggered, [&](){ aboutDlg_->show(); });
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

void MainWindow::setWindowStyle()
{
    QFont font = this->font();
    font.setPointSize(13);
    ui->addList->setFont(font);
    QFile file(":/qss/psblack.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

bool MainWindow::getConfPath(QString& iniPath, QString& confDirPath) {
    QString iniFileName = "conf.ini";
    QString iniFileDir = "conf";
    QString dotPath = QDir::currentPath();;
    QDir dir1(dotPath);
    while(!dir1.exists(iniFileDir) && !dir1.isRoot()){
        dir1.cdUp();
    }
    if(dir1.exists(iniFileDir)){
        dir1.cd(iniFileDir);
        if(dir1.exists(iniFileName)){
            confDirPath =dir1.absolutePath();
            iniPath = dir1.absolutePath()+"/"+iniFileName;
            return true;
        }
    }
    return false;
}
void MainWindow::appendTextToLog(QString log)
{
    ui->logText->append(log.toUtf8());
}


