#ifndef QTDIRINFO_H
#define QTDIRINFO_H

#include <QString>
#include <QMap>
#include <QXmlStreamReader>
#include <QPair>

class QtDirInfo
{
public:
    enum LocationMethod { LocateQtDir, LocateQLibInfo, LocateCreator };

    static QString locate(LocationMethod method);
    static QString qtDirFromCreatorSettings();
};

class QtVersionReader
{
public:
    bool read();
    QMap<QString, QString> versionInfo;

private:
    QString                 readVariable();
    QPair<QString, QString> readValue();
    QString                 readValuemap();
    QPair<QString, QString> readData();

    QXmlStreamReader xml;
};

#endif // QTDIRINFO_H
