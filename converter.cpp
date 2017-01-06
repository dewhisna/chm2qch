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

#include "converter.h"
#include "chmfile.h"

#include <iostream>
#include <QXmlStreamWriter>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QDir>

Converter::Converter()
{
    quiet     = false;
    generate  = false;
    clean     = false;
    writeRoot = true;
}

void Converter::msg(const QString &s0, const QString &s1)
{
    if(!quiet)
        std::cout << s0.toStdString() << " " << s1.toStdString() << std::endl;
}

bool Converter::run()
{
    ChmFile chm;

    if(!chm.open(fileName))
    {
        std::cout << "ERROR: Cannot open " << fileName.toStdString();
        return false;
    }

    foreach(const QString &name, chm.objectList())
        writeFile(QDir::cleanPath(destDir + "/" + fileSystemName(name)), chm.objectData(name));

    QString qhpname = QFileInfo(fileName).completeBaseName() + ".qhp";

    if(nameSpace.isEmpty())
        nameSpace = namespaceFromTitle(QFileInfo(fileName).completeBaseName());

    writeQhp(QDir::cleanPath(destDir + "/" + qhpname), &chm, nameSpace, writeRoot);

    if(generate)
        runQhg(QDir::cleanPath(destDir + "/" + qhpname));

    if(clean)
        cleanFiles();

    msg("chm2qch finished.");
    return true;
}

QString Converter::fileSystemName(const QString &objname)
{
    QString str = objname;
    return str.remove(0, 1);
}

QString Converter::namespaceFromTitle(QString title, bool full)
{
    title.replace(" ", "");

    if(full)
        return QString("com.chm.") + title;
    else
        return title;
}

void Converter::writeFile(const QString &filename, const QByteArray &data)
{
    QFileInfo fi(filename);
    msg("Extracting", fi.fileName());

    QDir dd(fi.path());
    if(!dd.exists())
        dd.mkpath(fi.path());

    QFile f(filename);

    if(!f.open(QFile::WriteOnly))
    {
        msg("ERROR: Cannot create", filename);
        return;
    }

    f.write(data);
    generatedFiles.append(filename);
}

void Converter::writeQhp(const QString &filename, ChmFile *chm, const QString &nameSpace, bool writeRoot)
{
    QFileInfo fi(filename);
    msg("Writing Qt Help project", fi.fileName());

    QFile f(filename);
    f.open(QFile::WriteOnly);

    QXmlStreamWriter xml(&f);

    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("QtHelpProject");
    xml.writeAttribute("version", "1.0");

    xml.writeTextElement("namespace", nameSpace);
    xml.writeTextElement("virtualFolder", "doc"); //???

    xml.writeStartElement("customFilter");
    xml.writeAttribute("name", chm->title());
    xml.writeTextElement("filterAttribute", namespaceFromTitle(fi.baseName(), false));
    xml.writeEndElement();

    xml.writeStartElement("filterSection");
    xml.writeTextElement("filterAttribute", namespaceFromTitle(fi.baseName(), false));

    xml.writeStartElement("toc");
    chm->writeToc(xml, writeRoot);
    xml.writeEndElement();//toc

    xml.writeStartElement("keywords");
    chm->writeIndex(xml);
    xml.writeEndElement();//keywords

    xml.writeStartElement("files");

    foreach(const QString &name, chm->objectList())
        xml.writeTextElement("file", fileSystemName(name));

    xml.writeEndElement();//files

    xml.writeEndElement();//filterSection

    xml.writeEndElement();//QtHelpProject
    xml.writeEndDocument();

    generatedFiles.append(filename);
}

void Converter::runQhg(const QString &qhpname)
{
    msg("Running qhelpgenerator:");

    QProcess qhg;

    if(!quiet)
        qhg.setProcessChannelMode(QProcess::ForwardedChannels);

    qhg.start("qhelpgenerator", {qhpname});
    qhg.waitForFinished(-1);
}

void Converter::cleanFiles()
{
    msg("Cleaning files...");
    foreach(const QString &name, generatedFiles)
    {
        QFile f(name);
        f.remove();
    }
}

