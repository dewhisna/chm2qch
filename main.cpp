#include "chmfile.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QStringList>
#include <iostream>

bool quiet = false;
QStringList generatedFiles;

void msg(const QString &s0, const QString &s1 = QString())
{
    if(!quiet)
        std::cout << s0.toStdString() << " " << s1.toStdString() << std::endl;
}

QString fileSystemName(const QString &objname)
{
    QString str = objname;
    return str.remove(0, 1);
}

QString namespaceFromTitle(QString title, bool full = true)
{
    title.replace(" ", "");

    if(full)
        return QString("com.chm.") + title;
    else
        return title;
}

void writeFile(const QString &filename, const QByteArray &data)
{
    QFileInfo fi(filename);
    msg("Extracting", fi.fileName());

    QDir dd(fi.path());
    if(!dd.exists())
        dd.mkpath(fi.path());

    QFile f(filename);
    f.open(QFile::WriteOnly);
    f.write(data);
    generatedFiles.append(filename);
}

void writeQhp(const QString &filename, ChmFile *chm, const QString &nameSpace, bool writeRoot = true)
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

void runQhg(const QString &qhpname)
{
    msg("Running qhelpgenerator:");

    QProcess qhg;

    if(!quiet)
        qhg.setProcessChannelMode(QProcess::ForwardedChannels);

    qhg.start("qhelpgenerator", {qhpname});
    qhg.waitForFinished(-1);
}

void cleanFiles()
{
    msg("Cleaning files...");
    foreach(const QString &name, generatedFiles)
    {
        QFile f(name);
        f.remove();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName("Mitrich Software");
    app.setApplicationName("chm2qch");
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Converts CHM files to QCH format.");
    parser.addHelpOption();
    parser.addPositionalArgument("input", "Input CHM file");
    parser.addOptions({
        {{"g", "generate" }, "Run qhelpgenerator to produce QCH file"},
        {{"r", "no-root"  }, "Do not write root contents section"},
        {{"q", "quiet"    }, "Quiet mode. Do not write any messages to stdout" },
        {{"n", "namespace"}, "Set documentation namespace to <name>", "name"},
        {{"c", "clean"    }, "Delete immediate files after running qhelpgenerator"},
        {{"d", "directory"}, "Set target directory to <dir>", "dir"}
        //{{"t", "temp"     }, "Use Temporary directory as dest dir" }
        //{{"o", "outfile"  }, "Set output file name to <out>", "out"},
    });
    parser.process(app);

    quiet = parser.isSet("q");
    QString filename = parser.positionalArguments().value(0);
    QString destDir = parser.value("d");

    if(destDir.isEmpty())
        destDir = QDir::currentPath();

    if(filename.isEmpty())
    {
        std::cout << "No input file specified." << std::endl;
        parser.showHelp();
        return 0;
    }

    ChmFile chm;

    if(!chm.open(filename))
    {
        std::cout << "Cannot open " << filename.toStdString();
        return 1;
    }

    foreach(const QString &name, chm.objectList())
        writeFile(QDir::cleanPath(destDir + "/" + fileSystemName(name)), chm.objectData(name));

    QString qhpname = QFileInfo(filename).completeBaseName() + ".qhp";
    QString nameSpace = parser.value("n");
    bool writeRoot = !parser.isSet("r");

    if(nameSpace.isEmpty())
        nameSpace = namespaceFromTitle(QFileInfo(filename).completeBaseName());

    writeQhp(QDir::cleanPath(destDir + "/" + qhpname), &chm, nameSpace, writeRoot);

    if(parser.isSet("g"))
        runQhg(QDir::cleanPath(destDir + "/" + qhpname));

    if(parser.isSet("c"))
        cleanFiles();

    std::cout << "chm2qch finished.";
    return app.exec();
}
