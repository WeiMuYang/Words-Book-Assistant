#ifndef DATA_TYPE_H
#define DATA_TYPE_H
#include <QString>
#include <QStringList>
#include <QVector>

struct WordSentInfo {
    QString m_WordSent;
    QString m_Phonetic_UK;
    QStringList m_Translation; // n. 名词
};

struct IniInfo{
    QString m_IniPath;
    QString m_HostName;
    QStringList m_RecentFile;
    QString m_Version;
    QString m_Date;
};

struct RepoNamePath {
    QString m_Name;
    QString m_Path;
};

struct UserInfo{
    QString m_UserPath;
    QVector<RepoNamePath> m_RepoPathList;
    QString m_TyporaPath;
    QString m_VsCodePath;
    QString m_CambridgeWordWeb;
    QString m_CambridgeSentWeb;
    QString m_TranslateWeb;
    QString m_PronunciationWeb;
    QString m_Symbols;
};


typedef enum {
    High, Low
}ScreenRes;

typedef enum {
    None, SubCombox, NumSpinBox
}BoxSelect;


#endif // DATA_TYPE_H
