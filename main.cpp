#include "converter.h"

#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>


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
        {{"c", "clean"    }, "Delete intermediate files after running qhelpgenerator (use with -g)"},
        {{"d", "directory"}, "Set target directory to <dir>", "dir"}
        //{{"t", "temp"     }, "Use Temporary directory as dest dir" }
        //{{"o", "outfile"  }, "Set output file name to <out>", "out"},
    });
    parser.process(app);

    QString fileName = parser.positionalArguments().value(0);
    QString destDir = parser.value("d");

    if(fileName.isEmpty())
    {
        std::cout << "No input file specified." << std::endl;
        parser.showHelp();
        return 0;
    }

    if(destDir.isEmpty())
        destDir = QDir::currentPath();

    Converter converter;
    converter.fileName  = fileName;
    converter.destDir   = destDir;
    converter.quiet     = parser.isSet("q");
    converter.generate  = parser.isSet("g");
    converter.clean     = parser.isSet("c");
    converter.nameSpace = parser.value("n");
    converter.writeRoot = !parser.isSet("r");

    if(converter.run())
        return EXIT_FAILURE;

    return app.exec();
}
