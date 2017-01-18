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
