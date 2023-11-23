#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDesktopWidget>
#include "data_type.h"

int printscreeninfo()
{
    QDesktopWidget *dwsktopwidget = QApplication::desktop();
    QRect deskrect = dwsktopwidget->availableGeometry();
    QRect screenrect = dwsktopwidget->screenGeometry();
    return deskrect.width();
}

double multiple = 2;  // 1 or 12/9


MainWindow::MainWindow(QWidget *parent,QApplication* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont font;

    multiple_ = 2;
    // 1. 设置界面风格
    setWindowStyle();
    font = this->font();
    font.setPointSize(13);
    this->setFont(font);
    // 2. 获取配置文件
    operateFile_ = new FileOperation(this);
    initConfInfo();
    // 3. 添加Ctrl + C的过滤
    winEventFilter_->setSymbols(userInfo_.m_Symbols);
    app->installNativeEventFilter(winEventFilter_ = new WinEventFilter);
    connect(winEventFilter_,&WinEventFilter::sendWords,this, &MainWindow::getWordsSlot);

    netWorkAccess_ = new NetworkAccess(this);
    netWorkAccess_->setTranslateWeb(userInfo_.m_TranslateWeb);
    connect(netWorkAccess_,&NetworkAccess::sendWordInfo,this, &MainWindow::addWordListSlot);
    connect(netWorkAccess_,&NetworkAccess::sendSentenceInfo,this, &MainWindow::addSentenceListSlot);

    aboutDlg_ = new AboutDialog(this);
    openExPro_ = new OpenExProgram;
    openExPro_->setTyporaPath(userInfo_.m_TyporaPath);
    openExPro_->setVscodePath(userInfo_.m_VsCodePath);
    initActions();

    // 鼠标掠过事件
    ui->addList->installEventFilter(this);
    ui->addList->setMouseTracking(true);
    connect(ui->addList,&QListWidget::itemEntered,this, &MainWindow::itemEnteredSlot);
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

void MainWindow::initWordSentList() {
    ui->addList->setViewMode(QListView::ListMode);
    ui->addList->setIconSize(QSize(40 ,20 * multiple_));
    ui->addList->setSpacing(4);
}

void MainWindow::updateListWgt()
{
    ui->addList->clear();

    for(int i = 0; i < wordSentList_.size(); ++i){
        QListWidgetItem *pItem = new QListWidgetItem;
        QString data = wordSentList_.at(i).m_WordSent;
        pItem->setSizeHint(QSize(20,20 * multiple_));
        pItem->setText(data);
        pItem->setToolTip(data);

        ui->addList->addItem(pItem);
    }
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

void MainWindow::addWordListSlot(WordSentInfo wordInfo) {
    wordSentList_.append(wordInfo);
    QListWidgetItem *pItem = new QListWidgetItem;
    QString data = wordInfo.m_WordSent;
    pItem->setSizeHint(QSize(20,20 * multiple_));
    pItem->setText(data);
    pItem->setToolTip(data);

    ui->addList->addItem(pItem);
}

void MainWindow::addSentenceListSlot(WordSentInfo Sentence) {
    wordSentList_.append(Sentence);
    QListWidgetItem *pItem = new QListWidgetItem;
    QString data = Sentence.m_WordSent;
    pItem->setSizeHint(QSize(20,20 * multiple_));
    pItem->setText(data);
    pItem->setToolTip(data);

    ui->addList->addItem(pItem);
}

void MainWindow::appendWordInfoSlot(WordSentInfo wordInfo) {

    if(operateFile_->appendWord(wordInfo)) {
        qDebug() << "append Word succeed ";
    }else{
        qDebug() << "append Word fauiled ";
    }
}

void MainWindow::appendSentenceInfoSlot(WordSentInfo wordInfo) {

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

void MainWindow::itemEnteredSlot(QListWidgetItem *item)
{
    QString name = item->text();
    WordSentInfo wordInfo;
    for (int i = 0; i < wordSentList_.size(); ++i) {
        if(wordSentList_.at(i).m_WordSent == name) {
            wordInfo = wordSentList_.at(i);
            break;
        }
    }
    for(int i = 0; i < wordSentList_.size(); ++i ) {
        if(ui->addList->item(i)->text() == name){
            ui->textEdit->clear();
            if(-1 == wordInfo.m_WordSent.indexOf(" ")) {
                ui->textEdit->append("<font color=\"#00FFFF\">"
                                     + wordInfo.m_WordSent + "</font> /"
                                     + wordInfo.m_Phonetic_UK +"/");
            } else {
                ui->textEdit->append("<font color=\"#00FFFF\">"
                                     + wordInfo.m_WordSent + "</font>");
            }
            for(int i = 0; i < wordInfo.m_Translation.size(); ++i) {
                ui->textEdit->append("- " + wordInfo.m_Translation.at(i));
            }
            break;
        }
    }
}
