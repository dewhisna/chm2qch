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

#include <iostream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>

#ifdef USE_GUI
#include "dialog.h"
#include <QApplication>
#endif

int main(int argc, char *argv[])
{
#ifdef USE_GUI
    QApplication app(argc, argv);
#else
    QCoreApplication app(argc, argv);
#endif
    app.setOrganizationName("Mitrich Software");
    app.setApplicationName("chm2qch");
    app.setApplicationVersion("1.2.0");

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
        {{"d", "directory"}, "Set target directory to <dir>", "dir"},
        {{"q", "qtdir"    }, "Set Qt binaries location to <dir>", "dir" }
        //{{"t", "temp"     }, "Use Temporary directory as dest dir" }
        //{{"o", "outfile"  }, "Set output file name to <out>", "out"},
    });
    parser.process(app);

    Converter converter;
    converter.quiet     = parser.isSet("q");
    converter.generate  = parser.isSet("g");
    converter.clean     = parser.isSet("c");
    converter.nameSpace = parser.value("n");
    converter.writeRoot = !parser.isSet("r");
    converter.qtDir     = parser.value("q");

    QString destDir = parser.value("d");

    if(destDir.isEmpty())
        destDir = QDir::currentPath();

    converter.destDir = destDir;

    QString fileName = parser.positionalArguments().value(0);

#ifdef USE_GUI
    Dialog dialog(&converter);
#endif

    if(fileName.isEmpty())
    {
#ifdef USE_GUI
        dialog.show();
#else
        std::cout << "No input file specified." << std::endl;
        parser.showHelp();
        return 0;
#endif
    }
    else
    {
        converter.fileName  = fileName;

        if(!converter.run())
            return EXIT_FAILURE;
    }

#ifdef USE_GUI
    return app.exec();
#else
    return 0;
#endif
}
