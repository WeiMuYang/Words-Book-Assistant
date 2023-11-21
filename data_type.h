#ifndef DATA_TYPE_H
#define DATA_TYPE_H
#include <QString>
#include <QStringList>

struct WordInfo {
    QString m_word;
    QString m_Phonetic_UK;
    QStringList m_Translation; // n. 名词
};

#endif // DATA_TYPE_H
