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

#ifndef CHMFILE_H
#define CHMFILE_H

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamWriter>

#define CHMINFO_DEFAULT_TOPIC 2
#define CHMINFO_TITLE         3
#define CHMINFO_LOCALE        4

struct chmFile;
struct chmUnitInfo;

class ChmFile
{
public:
    ChmFile();
    ChmFile(const QString &filename);
    ~ChmFile();

    bool open(const QString &filename);
    void close();

    QStringList objectList();
    QByteArray objectData(const QString &name);

    QString title();
    QString homeUrl();
    QString encoding();

    void writeToc(QXmlStreamWriter &xml, bool writeRoot = true);
    void writeIndex(QXmlStreamWriter &xml);

private:
    QString tocFileName() const;
    QString idxFileName() const;
    void    readSystemData();
    QString fixUrl(QString url);

    chmFile *handle;
    QStringList objNames;
    QString titleStr;
    QString defaultTopic;
    unsigned short lcid;

    friend int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context);
    //friend class HhParser;
};

int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context);

#endif // CHMFILE_H
