#include "qtdirinfo.h"

#include <QDir>
#include <QLibraryInfo>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>

QString QtDirInfo::locate(LocationMethod method)
{
    switch(method)
    {
    case LocateQtDir   : return QDir::cleanPath(QString(qgetenv("QTDIR")) + "/bin/");
    case LocateQLibInfo: return QLibraryInfo::location(QLibraryInfo::BinariesPath);
    case LocateCreator : return qtDirFromCreatorSettings();
    default: return QString();
    }
}

QString QtDirInfo::qtDirFromCreatorSettings()
{
    QtVersionReader qv;

    if(!qv.read())
        return QString();

    QString ver = qv.versionInfo.value("Version");
    QString qmakePath = qv.versionInfo.value(QString("QtVersion.") + ver);

    QFileInfo fi(qmakePath);

    if(fi.exists())
        return fi.path();
    //else
    QMapIterator<QString, QString> i(qv.versionInfo);

    while(i.hasNext())
    {
        i.next();
        QFileInfo fi(i.value());

        if(fi.exists())
            return fi.path();
    }
    //none found
    return QString();
}

bool QtVersionReader::read()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, "QtProject", "QtCreator", NULL);
    QString filename =  QFileInfo(s.fileName()).path() + "/qtcreator/qtversion.xml";
    QFile f(filename);

    if(!f.open(QFile::ReadOnly))
        return false;

    xml.setDevice(&f);
    xml.readNextStartElement();

    while(xml.readNextStartElement())
    {
        if(xml.name() == "data")
        {
            QPair<QString, QString> d = readData();
            versionInfo.insert(d.first, d.second);
        }
        else
            xml.skipCurrentElement();
    }

    return true;
}

QString QtVersionReader::readVariable()
{
    return xml.readElementText();
}

QPair<QString, QString> QtVersionReader::readValue()
{
    QString key = xml.attributes().value("key").toString();
    QString value = xml.readElementText();
    return QPair<QString, QString>(key, value);
}

QString QtVersionReader::readValuemap()
{
    QString s;

    while(xml.readNextStartElement())
    {
        if(xml.name() == "value")
        {
            QPair<QString, QString> v = readValue();
            if(v.first == "QMakePath")
                s = v.second;
        }
        else
            xml.skipCurrentElement();
    }

    return s;
}

QPair<QString, QString> QtVersionReader::readData()
{
    QPair<QString, QString> d;

    while(xml.readNextStartElement())
    {
        if(xml.name() == "variable")
            d.first = readVariable();
        else if(xml.name() == "valuemap")
            d.second = readValuemap();
        else if(xml.name() == "value")
            d.second = readValue().second;
        else
            xml.skipCurrentElement();
    }

    return d;
}
