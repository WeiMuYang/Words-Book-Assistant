#include "create_markdown_dir.h"
#include "ui_create_markdown_dir.h"
#include <QDir>
#include <QDebug>

CreateMarkdownAndSubDir::CreateMarkdownAndSubDir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateMarkdown)
{
    ui->setupUi(this);
}

CreateMarkdownAndSubDir::~CreateMarkdownAndSubDir()
{
    delete ui;
}

bool CreateMarkdownAndSubDir::isMarkdownFile(QString fileName){
    if(fileName.size() >= 5 && fileName.right(3) == ".md"){
        return true;
    }
    return false;
}

void CreateMarkdownAndSubDir::updateMarkdownWgt()
{
    QDir dir(subDirPath_);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    int max = -1;
    QString templateFileName;
    for(int i = 0; i < list.size(); ++i){
        if(isMarkdownFile(list.at(i).fileName())){
            QStringList nameArr = list.at(i).fileName().split("-");
            int num = nameArr.at(0).toInt();
            if(num > max){
                max = num;
            }
            if(num == 0 && list.at(i).fileName().right(5) != "um.md"){
                templateFileName = list.at(i).fileName();
            }
        }
    }
    ui->numOldMarkdownSpinBox->setValue(max);
    ui->numNewMarkdownSpinBox->setValue(max + 1);
}

void CreateMarkdownAndSubDir::updateSubDirWgt()
{
    QDir dir(repoPath_);
    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QVector<int> numArr;
    for(int i = 0; i < list.size(); ++i){
        if(list.at(i).isDir()){
            QStringList nameArr = list.at(i).fileName().split("-");
            numArr.append(nameArr.at(0).toInt());
        }
    }
    int num = -1;
    std::sort(numArr.begin(), numArr.end());
    for(int i = 0; i < numArr.size() - 1; ++i) {
        if(numArr.at(i+1) - numArr.at(i) > 1  ) {
            num = numArr.at(i) + 1;
            break;
        }
    }
    if(-1 == num) {
        num = numArr.last() + 1;
    }
    ui->numSubDirSpinBox->setValue(num);
    emit ui->numSubDirSpinBox->valueChanged(num);
}

void CreateMarkdownAndSubDir::updateRepoWgt()
{
    QDir dirParent(repoPath_);
    dirParent.cdUp();
    QFileInfoList list = dirParent.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QVector<int> numArr;
    for(int i = 0; i < list.size(); ++i){
        if(list.at(i).isDir()){
            QStringList nameArr = list.at(i).fileName().split("-");
            numArr.append(nameArr.at(0).toInt());
        }
    }
    int num = -1;
    std::sort(numArr.begin(), numArr.end());
    for(int i = 0; i < numArr.size() - 1; ++i) {
        if(numArr.at(i+1) - numArr.at(i) > 1  ) {
            num = numArr.at(i) + 1;
            break;
        }
    }
    if(-1 == num) {
        num = numArr.last() + 1;
    }
    ui->numNewRepoSpinBox->setValue(num);
}

void CreateMarkdownAndSubDir::showWindow() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->fileRadioBtn->setChecked(true);
    ui->newDirFileNameEdit->setText("");
    ui->newFileNameEdit->setText("");
    ui->newRepoNameEdit->setText("");

    initSize();
    updateMarkdownWgt();
    updateSubDirWgt();
    updateRepoWgt();
    show();
}

void CreateMarkdownAndSubDir::initSize() {
    double widthIn4K = 830;
    double heightIn4K = 400;
    double zoom = 1;
    if(width_ < 3840) {
        zoom = 1.2;
    }
    // 830 400
    // 宽高比
    double WindowAspect = heightIn4K / widthIn4K;
    // 占屏比
    double Proportion = widthIn4K / 3840.0;
    // 宽 高
    int width = width_ * Proportion ;
    int height = width * WindowAspect;
    setMinimumSize(QSize(width, height) * zoom);
    this->resize(QSize(width, height) * zoom);
}


void CreateMarkdownAndSubDir::on_fileRadioBtn_clicked(bool checked)
{
    if(checked) {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void CreateMarkdownAndSubDir::on_dirRadioBtn_clicked(bool checked)
{
    if(checked) {
        ui->stackedWidget->setCurrentIndex(1);
    }
}


void CreateMarkdownAndSubDir::numOldMarkdownSpinBoxStatus(int flags) {
    if(flags == 1){
        ui->numOldMarkdownSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}");
        ui->templeFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}");
        ui->newFileNameEdit->setEnabled(true);
    }else if(flags == 2) {
        ui->numOldMarkdownSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #ffff00\n}\n");
        ui->templeFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}\nQLineEdit{\nborder: 1px solid #ffff00\n}\n");
        ui->newFileNameEdit->setEnabled(true);
    }else{
        ui->numOldMarkdownSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #ff0000\n}\n");
        ui->templeFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}\nQLineEdit{\nborder: 1px solid #ff0000\n}\n");
        ui->newFileNameEdit->setEnabled(false);
    }
}

void CreateMarkdownAndSubDir::numNewMarkdownSpinBoxStatus(int flags) {
    if(flags == 1){
        ui->numNewMarkdownSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}");
        ui->newFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}");
        ui->newFileNameEdit->setEnabled(true);
    }else{
        ui->numNewMarkdownSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #ff0000\n}\n");
        ui->newFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}\nQLineEdit{\nborder: 1px solid #ff0000\n}\n");
        ui->newFileNameEdit->setEnabled(false);
    }
}

void CreateMarkdownAndSubDir::numDirSpinBoxStatus(int flags) {
    if(flags == 1){
        ui->numSubDirSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}");
        ui->newDirFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}");
        ui->newDirFileNameEdit->setEnabled(true);
    }else{
        ui->numSubDirSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #ff0000\n}\n");
        ui->newDirFileNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}\nQLineEdit{\nborder: 1px solid #ff0000\n}\n");
        ui->newDirFileNameEdit->setEnabled(false);
    }
}

void CreateMarkdownAndSubDir::numRepoSpinBoxStatus(int flags) {
    if(flags == 1){
        ui->numNewRepoSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}");
        ui->newRepoNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}");
        ui->newRepoNameEdit->setEnabled(true);
    }else{
        ui->numNewRepoSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #ff0000\n}\n");
        ui->newRepoNameEdit->setStyleSheet("QLineEdit::up-button,QLineEdit::down-button\n{\n	width:0px;\n}\nQLineEdit{\nborder: 1px solid #ff0000\n}\n");
        ui->newRepoNameEdit->setEnabled(false);
    }
}

void CreateMarkdownAndSubDir::on_numOldMarkdownSpinBox_valueChanged(int fileNum)
{
    QDir dir(subDirPath_);
    QString fileName;
    QStringList filters;
    QString nameNum = QString("%1").arg(fileNum, 2, 10, QLatin1Char('0'))+"*";
    filters << nameNum;
    dir.setFilter(QDir::AllEntries);
    dir.setNameFilters(filters);
    QFileInfoList fileInfoList = dir.entryInfoList();
    if(fileInfoList.size() > 1) {
        // 编号为fileNum的文件有多个
        QString msg;
        msg = QString("包含") + QString::number(fileInfoList.size())+ QString("个序号为： \"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\" 的markdown文件：\n");
        for(auto it = fileInfoList.begin(); it < fileInfoList.end(); it++){
            msg += it->fileName() + ", ";
        }
        msg[msg.size()-2] = '!';
        qDebug() << msg;
        numOldMarkdownSpinBoxStatus(2);
    }else if(fileInfoList.size() < 1){
        // 编号为fileNum的文件没有
        QString msg;
        msg = QString("序号为：\"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\"的文件or目录不存在！").toUtf8();
        if(fileNum == 0) {
            ui->templeFileNameEdit->setText("没有模板文件!");
            numOldMarkdownSpinBoxStatus(0);
        } else {
            numOldMarkdownSpinBoxStatus(1);
            ui->numOldMarkdownSpinBox->setValue(0);
        }
        return;
    }

    fileName = fileInfoList.last().fileName();
    ui->templeFileNameEdit->setText(fileName.split(QString("%1").arg(fileNum, 2, 10, QLatin1Char('0'))+"-").last());
    numOldMarkdownSpinBoxStatus(1);
}


void CreateMarkdownAndSubDir::on_numSubDirSpinBox_valueChanged(int fileNum)
{
    QDir dir(repoPath_);
    QStringList filters;
    QString nameNum = QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + "*";
    filters << nameNum;
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setNameFilters(filters);
    QFileInfoList fileInfoList = dir.entryInfoList();
    if(fileInfoList.size() > 0) {
        QString msg;
        msg = QString("序号为：\"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\"的目录被占用！").toUtf8();
        qDebug() << msg;
        numDirSpinBoxStatus(0);
        ui->oldDirFileNameEdit->setText(fileInfoList.first().fileName());
        return;
    }
    ui->oldDirFileNameEdit->setText("编号\"" + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + "\"没有被占用。");
    numDirSpinBoxStatus(1);
}

void CreateMarkdownAndSubDir::on_numNewMarkdownSpinBox_valueChanged(int fileNum)
{
    QDir dir(subDirPath_);
    QStringList filters;
    QString nameNum = QString("%1").arg(fileNum, 2, 10, QLatin1Char('0'))+"*";
    filters << nameNum;
    dir.setFilter(QDir::AllEntries);
    dir.setNameFilters(filters);
    QFileInfoList fileInfoList = dir.entryInfoList();
    if(fileInfoList.size() > 0) {
        // 有编号为fileNum的文件
        QString msg;
        msg = QString("序号为：\"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\"的文件被占用！").toUtf8();
        qDebug() << msg;
        numNewMarkdownSpinBoxStatus(0);
        return;
    }
    numNewMarkdownSpinBoxStatus(1);
}

void CreateMarkdownAndSubDir::on_yesPbn_clicked()
{
    if(ui->fileRadioBtn->isChecked() && ui->newFileNameEdit->isEnabled()) {
        QString newFilePathAbs;
        if(createMarkdown(newFilePathAbs)){
            emit sigCreateType(1, newFilePathAbs);
        }
    }else if(ui->dirRadioBtn->isChecked() && ui->newDirFileNameEdit->isEnabled()){
        QString newSunDir;
        if(createSubDir(newSunDir)) {
            emit sigCreateType(2, newSunDir);
        }
    }else if(ui->repoRadioBtn->isChecked() && ui->newRepoNameEdit->isEnabled()){
        emit sigCreateMarkdownAndDirLog(QString("目前不支持创建!"));
//        QString newRepoDir;
//        if(createRepo(newRepoDir)) {
//            emit sigCreateType(3, newRepoDir);
//        }
    }else{
        emit sigCreateMarkdownAndDirLog(QString("取消创建!"));
    }

    this->close();
}

QString CreateMarkdownAndSubDir::getMarkdownTag(const QString &markdownPathAbs) {
    QString text = "> ######  _标签:_   ![](https://img.shields.io/badge/技术类-yellowgreen.svg)   ![ ](https://img.shields.io/badge/Protobuf-编译和使用-blue.svg)    [![](https://img.shields.io/badge/链接-github仓库-brightgreen.svg)](https://github.com/protocolbuffers/protobuf#protocol-compiler-installation)    [![](https://img.shields.io/badge/链接-代码文件-orange.svg)](../02-code/)    [![](https://img.shields.io/badge/链接-本地仓库-orange.svg)](../04-repo/)    [![](https://img.shields.io/badge/链接-数据文件-orange.svg)](../03-data/)  \n"   \
                   ">  \n\n\n";
    QFile file(markdownPathAbs);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<< "Can't open the file!";
        return text;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(line.contains("标签")) {
            text.clear();
            text = line;
            break;
        }
    }
    file.close();
    return text;
}

bool CreateMarkdownAndSubDir::createMarkdown(QString& path) {
    if(ui->newFileNameEdit->text().isEmpty()) {
        emit sigCreateMarkdownAndDirLog(QString("新建文件名不能为空!"));
        return false;
    }
    QString tempFileName = QString("%1").arg(ui->numOldMarkdownSpinBox->value(), 2, 10,
                                             QLatin1Char('0')) + "-" + ui->templeFileNameEdit->text();
    QString fileName = QString("%1").arg(ui->numNewMarkdownSpinBox->value(), 2, 10,
                                         QLatin1Char('0')) + "-" + ui->newFileNameEdit->text() + ".md";
    QString newFilePathAbs = subDirPath_ + "/" + fileName;
    path = newFilePathAbs;
    QString templateFileAbs = subDirPath_ + "/" + tempFileName;
    QFile file(path);
    if (file.exists()) {
        emit sigCreateMarkdownAndDirLog(QString("新建文件已存在！"));
        return false;
    }
    file.open(QIODevice::WriteOnly);
    QString text = "# [" + ui->newFileNameEdit->text() + "](./)    \n";
    text += getMarkdownTag(templateFileAbs);
    QByteArray str = text.toUtf8();
    file.write(str);
    file.close();
    return true;
}

bool CreateMarkdownAndSubDir::createSubDir(QString& path)
{
    if(ui->newDirFileNameEdit->text().isEmpty()){
        emit sigCreateMarkdownAndDirLog(QString("新建子目录不能为空!"));
        return false;
    }
    QString dirName = QString("%1").arg(ui->numSubDirSpinBox->value(), 2, 10,
                                        QLatin1Char('0')) + "-" + ui->newDirFileNameEdit->text();
    QString newDirPathAbs = repoPath_ + "/" + dirName;
    path = newDirPathAbs;
    if(createSubDir(repoPath_, dirName)) {
        emit sigCreateMarkdownAndDirLog(newDirPathAbs + QString(" Create Success!"));
        createMarkdownTemple(newDirPathAbs);
    }else {
        emit sigCreateMarkdownAndDirLog(newDirPathAbs + QString(" Create failed!"));
        return false;
    }
    return true;
}

bool CreateMarkdownAndSubDir::createSubDir(const QString& repoPathAbs, const QString& dirName) {
    QDir newDir(repoPathAbs);
    QString newDirPathAbs = repoPathAbs + "/" + dirName;
    if(newDir.mkpath(newDirPathAbs)) {
        emit sigCreateMarkdownAndDirLog(newDirPathAbs + QString(" Create Success!"));
        createMarkdownTemple(newDirPathAbs);
        return true;
    }
    return false;
}

bool CreateMarkdownAndSubDir::createMarkdownTemple(const QString& dirPathAbs) {
    QString path = dirPathAbs + "/00-模板文件.md";
    QFile file(path);
    if (file.exists()) {
        emit sigCreateMarkdownAndDirLog(QString("新建文件已存在！"));
        return false;
    }
    file.open(QIODevice::WriteOnly);
    QString text = "# [新建文件](./)  [img](./img)   \n" \
                   "\n" \
        "> ######  _标签:_   ![](https://img.shields.io/badge/技术类-yellowgreen.svg)   ![ ](https://img.shields.io/badge/Protobuf-编译和使用-blue.svg)    [![](https://img.shields.io/badge/链接-github仓库-brightgreen.svg)](https://github.com/protocolbuffers/protobuf#protocol-compiler-installation)    [![](https://img.shields.io/badge/链接-代码文件-orange.svg)](../02-code/)    [![](https://img.shields.io/badge/链接-本地仓库-orange.svg)](../04-repo/)    [![](https://img.shields.io/badge/链接-数据文件-orange.svg)](../03-data/)  \n"   \
                   ">  \n\n\n";
    QByteArray str = text.toUtf8();
    file.write(str);
    emit sigCreateMarkdownAndDirLog(QString("Create File") + path + "\nCreate File Success");
    file.close();
    return true;
}

bool CreateMarkdownAndSubDir::createReadMe(const QString& dirPathAbs, int num,const QString& repoName) {
    QString repoFileName = QString("%1").arg(num, 2, 10, QLatin1Char('0')) + "-" + repoName;
    QString path = dirPathAbs + "/" + repoFileName + ".md";
    QFile file(path);
    if (file.exists()) {
        emit sigCreateMarkdownAndDirLog(QString("新建文件已存在！"));
        return false;
    }
    file.open(QIODevice::WriteOnly);  //  + repoName
    QString text = "# [" + repoName + "](./)   \n|序号|名称|内容|备注|\n|:-:|:-:|:- |:-:|\n|0|[Book](./00-book)|||\n";
    text += "|1|[Study](./01-study)|||\n|2|[Code](./02-code)|||\n|3|[Data](./03-data)|||\n|10|[Others](./10-others)|||\n\n";
    text += "## 仓库简介   \n本仓库用于管理XXX.    \n## 资料介绍   \n\n";
    QByteArray str = text.toUtf8();
    file.write(str);
    emit sigCreateMarkdownAndDirLog(QString("Create File") + path + "\nCreate File Success");
    file.close();
    return true;
}

bool CreateMarkdownAndSubDir::createRepo(QString& path)
{
    if(ui->newRepoNameEdit->text().isEmpty()){
        emit sigCreateMarkdownAndDirLog(QString("新建子目录不能为空!"));
        return false;
    }
    QDir repoParent(repoPath_);
    repoParent.cdUp();
    QString repoName = QString("%1").arg(ui->numNewRepoSpinBox->value(), 2, 10,
                                        QLatin1Char('0')) + "-" + ui->newRepoNameEdit->text();
    QString newDirPathAbs = repoParent.absolutePath() + "/" + repoName;
    path = newDirPathAbs;

    if(repoParent.mkpath(newDirPathAbs)) {
        emit sigCreateMarkdownAndDirLog(newDirPathAbs + QString(" Create Success!"));
        createSubDir(newDirPathAbs, "10-others");
        createSubDir(newDirPathAbs, "03-data");
        createSubDir(newDirPathAbs, "02-code");
        createSubDir(newDirPathAbs, "00-book");
        createSubDir(newDirPathAbs, "01-study");
        createReadMe(newDirPathAbs, ui->numNewRepoSpinBox->value() , ui->newRepoNameEdit->text());
    }else {
        emit sigCreateMarkdownAndDirLog(newDirPathAbs + QString(" Create failed!"));
        return false;
    }
    return true;
}

void CreateMarkdownAndSubDir::on_cancelPbn_clicked()
{
    emit sigCreateMarkdownAndDirLog(QString("取消创建!"));
    this->close();
}

void CreateMarkdownAndSubDir::on_openTempPbn_clicked()
{
    if(ui->fileRadioBtn->isChecked() && ui->newFileNameEdit->isEnabled()) {
        QString tempFileName = QString("%1").arg(ui->numOldMarkdownSpinBox->value(), 2, 10,
                                                 QLatin1Char('0')) + "-" + ui->templeFileNameEdit->text();
        QString templateFileAbs = subDirPath_ + "/" + tempFileName;
        emit sigOpenTempleMarkdown(templateFileAbs);
    }else{
        emit sigCreateMarkdownAndDirLog(QString("参考文件名字为空无法打开!"));
    }
}

void CreateMarkdownAndSubDir::on_numNewRepoSpinBox_valueChanged(int fileNum)
{
    QDir dir(repoPath_);
    dir.cdUp();
    QStringList filters;
    QString nameNum = QString("%1").arg(fileNum, 2, 10, QLatin1Char('0'))+"*";
    filters << nameNum;
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setNameFilters(filters);
    QFileInfoList fileInfoList = dir.entryInfoList();
    if(fileInfoList.size() > 0) {
        QString msg;
        msg = QString("序号为：\"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\"的目录被占用！").toUtf8();
        qDebug() << msg;
        numRepoSpinBoxStatus(0);
        ui->oldRepoFileNameEdit->setText(fileInfoList.first().fileName());
        return;
    }
    ui->oldRepoFileNameEdit->setText("编号\"" + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + "\"没有被占用。");
    numRepoSpinBoxStatus(1);
}


void CreateMarkdownAndSubDir::on_repoRadioBtn_clicked(bool checked)
{
    if(checked) {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

