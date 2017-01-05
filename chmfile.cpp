#include "chmfile.h"
#include "hhparser.h"

#include <chm_lib.h>
#include <QBuffer>
#include <QDebug>

ChmFile::ChmFile()
{
    handle = NULL;
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
        xml.writeAttribute("ref", toc[i].urls.value(0).toString());

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
    p.parse(tocFileName(), toc, false);

    for(int i = 0; i < toc.count(); i++)
    {
        xml.writeStartElement("keyword");
        xml.writeAttribute("name", toc[i].name);
        xml.writeAttribute("ref", toc[i].urls.value(0).toString());
        xml.writeEndElement();
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

    buffer.read((char *)&version, 4);

    while(!buffer.atEnd())
    {
        buffer.read((char *)&code, 2);
        buffer.read((char *)&length, 2);
        QByteArray data = buffer.read(length);

        if(code == 3)
            titleStr = QString(data);
        else if(code == 2)
            defaultTopic = QString(data);
    }
}

int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context)
{
    Q_UNUSED(h);
    ChmFile *chm = (ChmFile *)context;
    chm->objNames.append(QString(ui->path));
    return CHM_ENUMERATOR_CONTINUE;
}

