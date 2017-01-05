#include "chmfile.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QProcess>
#include <QFileInfo>
#include <iostream>

bool quiet = false;

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
    msg("Extracting", filename);

    QFile f(filename);
    f.open(QFile::WriteOnly);
    f.write(data);
}

void writeQhp(const QString &filename, ChmFile *chm, const QString &nameSpace, bool writeRoot = true)
{
    msg("Writing Qt Help project", filename);

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
    xml.writeTextElement("filterAttribute", namespaceFromTitle(chm->title(), false));
    xml.writeEndElement();

    xml.writeStartElement("filterSection");
    xml.writeTextElement("filterAttribute", namespaceFromTitle(chm->title(), false));

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
        {{"n", "namespace"}, "Set documentation namespace to <name>", "name"}
        //{{"c", "clean"    }, "Delete immediate files after running qhelpgenerator"},
        //{{"o", "outfile"  }, "Set output file name to <out>", "out"},
        //{{"d", "directory"}, "Set target directory to <dir>", "dir"},
    });
    parser.process(app);

    quiet = parser.isSet("q");
    QString filename = parser.positionalArguments().value(0);

    if(filename.isEmpty())
    {
        std::cout << "No input file specified." << std::endl;
        parser.showHelp();
        return 0;
    }

    ChmFile chm(filename);

    foreach(const QString &name, chm.objectList())
        writeFile(fileSystemName(name), chm.objectData(name));

    QString qhpname = QFileInfo(filename).completeBaseName() + ".qhp";
    QString nameSpace = parser.value("n");
    bool writeRoot = !parser.isSet("r");

    if(nameSpace.isEmpty())
        nameSpace = namespaceFromTitle(chm.title());

    writeQhp(qhpname, &chm, nameSpace, writeRoot);

    if(parser.isSet("g"))
        runQhg(qhpname);

    std::cout << "chm2qch finished.";
    return app.exec();
}
