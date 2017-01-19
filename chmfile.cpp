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

#include "chmfile.h"
#include "hhparser.h"
#include "ebook_chm_encoding.h"

#include <chm_lib.h>
#include <QBuffer>
#include <QRegExp>
#include <QTextCodec>
#include <QDebug>

ChmFile::ChmFile()
{
    handle = NULL;
    lcid = 0;
}

ChmFile::ChmFile(const QString &filename)
{
    open(filename);
}

ChmFile::~ChmFile()
{
    close();
}

bool ChmFile::open(const QString &filename)
{
    close();
    handle = chm_open(filename.toLocal8Bit().data());

    if(!handle)
        return false;

    chm_enumerate(handle, CHM_ENUMERATE_NORMAL | CHM_ENUMERATE_FILES, &enumChmContents, (void *)this);
    readSystemData();

    return true;
}

void ChmFile::close()
{
    if(handle)
    {
        chm_close(handle);
        handle = NULL;
    }
}

QStringList ChmFile::objectList()
{
    return objNames;
}

QByteArray ChmFile::objectData(const QString &name)
{
    chmUnitInfo info;

    if(chm_resolve_object(handle, name.toLocal8Bit().data(), &info) != CHM_RESOLVE_SUCCESS)
    {
        qDebug() << "ChmFile::objectData: chm_resolve_object" << name << "failed";
        return QByteArray();
    }

    QByteArray arr(info.length, 0);
    chm_retrieve_object(handle, &info, (unsigned char *)arr.data(), 0, info.length);
    return arr;
}

QString ChmFile::title()
{
    return titleStr;
}

QString ChmFile::homeUrl()
{
    return defaultTopic;
}

QString ChmFile::encoding()
{
    return Ebook_CHM_Encoding::guessByLCID(lcid);
}

void ChmFile::writeToc(QXmlStreamWriter &xml, bool writeRoot)
{
    QList<ParsedEntry> toc;
    HhParser p(this);
    p.parse(tocFileName(), toc, false);

    if(writeRoot)
    {
        xml.writeStartElement("section");
        xml.writeAttribute("title", title());
        xml.writeAttribute("ref", homeUrl());
    }

    for(int i = 0; i < toc.count(); i++)
    {
        bool hasChild = toc.value(i+1).indent > toc[i].indent;

        xml.writeStartElement("section");
        xml.writeAttribute("title", toc[i].name);
        xml.writeAttribute("ref", fixUrl(toc[i].urls.value(0).toString()));

        if(!hasChild)
            xml.writeEndElement();//section

        int d = toc[i].indent - toc.value(i+1).indent;

        if(d)
        {
            for(int j = 0; j < d; j++)
                xml.writeEndElement();//section
        }
    }

    if(writeRoot)
        xml.writeEndElement();//section (root)
}

void ChmFile::writeIndex(QXmlStreamWriter &xml)
{
    QList<ParsedEntry> toc;
    HhParser p(this);
    p.parse(idxFileName(), toc, false);

    for(int i = 0; i < toc.count(); i++)
    {
        QString name = toc[i].name;
        QString ref = fixUrl(toc[i].urls.value(0).toString());

        if((!name.isEmpty()) && (!ref.isEmpty()))
        {
            xml.writeStartElement("keyword");
            xml.writeAttribute("name", name);
            xml.writeAttribute("ref", ref);
            xml.writeEndElement();
        }
    }
}

QString ChmFile::tocFileName() const
{
    foreach(const QString &str, objNames)
        if(str.endsWith(".hhc", Qt::CaseInsensitive))
            return str;

    return QString();
}

QString ChmFile::idxFileName() const
{
    foreach(const QString &str, objNames)
        if(str.endsWith(".hhk", Qt::CaseInsensitive))
            return str;

    return QString();
}

void ChmFile::readSystemData()
{
    QByteArray array = objectData("/#SYSTEM");
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);

    unsigned int version;
    unsigned short code;
    unsigned short length;
    QByteArray titleData;

    buffer.read((char *)&version, 4);

    while(!buffer.atEnd())
    {
        buffer.read((char *)&code, 2);
        buffer.read((char *)&length, 2);
        QByteArray data = buffer.read(length);

        switch(code)
        {
        case CHMINFO_TITLE        : titleData = data; break;
        case CHMINFO_DEFAULT_TOPIC: defaultTopic = QString(data); break;
        case CHMINFO_LOCALE       : lcid = (short)(data[0] | (data[1] << 8)); break;
        }
    }

    QTextCodec *codec = QTextCodec::codecForName(encoding().toLatin1());
    titleStr = codec->toUnicode(titleData);
    titleStr.remove(QChar('\0'));
}

QString ChmFile::fixUrl(QString url)
{
    QRegExp re("ms-its:*.chm::/", Qt::CaseInsensitive, QRegExp::Wildcard);
    return url.remove(re);
}

int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context)
{
    Q_UNUSED(h);
    ChmFile *chm = (ChmFile *)context;
    chm->objNames.append(QString(ui->path));
    return CHM_ENUMERATOR_CONTINUE;
}

