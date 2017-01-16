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

#ifndef HHPARSER_H
#define HHPARSER_H

#include "helper_entitydecoder.h"

#include <QString>
#include <QUrl>
#include <QList>

class ChmFile;

class ParsedEntry
{
public:
    ParsedEntry();

    QString		name;
    QList<QUrl>	urls;
    int			indent;
    QString		seealso;
};

class HhParser
{
public:
    HhParser(ChmFile *f);
    ~HhParser();

    bool parse(const QString& fileName, QList<ParsedEntry>& data, bool asIndex );

private:
    int findStringInQuotes(const QString& tag, int offset, QString& value, bool firstquote, bool decodeentities) const;

    ChmFile *chm;
    HelperEntityDecoder m_htmlEntityDecoder;
};

#endif // HHPARSER_H
