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

    if(!converter.run())
        return EXIT_FAILURE;

    return app.exec();
}
