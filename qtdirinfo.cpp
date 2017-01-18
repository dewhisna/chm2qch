/*
 *  chm2qch - Tool for converting Windows CHM files to Qt Help format.
 *  Copyright (C) 2016 Mitrich Software, bitbucket.org/mitrich_k/chm2qch
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    if(!xml.readNextStartElement())
        return false;

    if(xml.name() != "qtcreator")
        return false;

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
