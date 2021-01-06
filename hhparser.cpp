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

#include "hhparser.h"
#include "chmfile.h"
#include <QTextCodec>


HhParser::HhParser(ChmFile *f)
{
    chm = f;
}

HhParser::~HhParser()
{
}

/*
 * Based on EBook_CHM::parseFileAndFillArray from Kchmviewer source.
 * (Copyright (C) 2004-2014 George Yunaev, gyunaev@ulduzsoft.com)
 */
bool HhParser::parse(const QString& fileName, QList< ParsedEntry >& data, bool asIndex )
{
    const int MAX_NEST_DEPTH = 256;

    QByteArray rawData = chm->objectData(fileName);
    QTextCodec *codec = QTextCodec::codecForName(chm->encoding().toLatin1());
    QString src = codec->toUnicode(rawData);
    m_htmlEntityDecoder.changeEncoding(codec);

    int pos = 0, indent = 0, root_indent_offset = 0;
    bool in_object = false, root_indent_offset_set = false;

    ParsedEntry entry;

    // Split the HHC file by HTML tags
    int stringlen = src.length();

    while ( pos < stringlen && (pos = src.indexOf ('<', pos)) != -1 )
    {
        int i, word_end = 0;

        for ( i = ++pos; i < stringlen; i++ )
        {
            // If a " or ' is found, skip to the next one.
            if ( (src[i] == '"' || src[i] == '\'') )
            {
                // find where quote ends, either by another quote, or by '>' symbol (some people don't know HTML)
                int nextpos = src.indexOf (src[i], i+1);
                if ( nextpos == -1 	&& (nextpos = src.indexOf ('>', i+1)) == -1 )
                {
                    qWarning ("EBook_CHMImpl::ParseHhcAndFillTree: corrupted TOC: %s", qPrintable( src.mid(i) ));
                    return false;
                }

                i =  nextpos;
            }
            else if ( src[i] == '>'  )
                break;
            else if ( !src[i].isLetterOrNumber() && src[i] != '/' && !word_end )
                word_end = i;
        }

        QString tagword, tag = src.mid (pos, i - pos);

        if ( word_end )
            tagword = src.mid (pos, word_end - pos).toLower();
        else
            tagword = tag.toLower();

        // <OBJECT type="text/sitemap"> - a topic entry
        if ( tagword == "object" && tag.indexOf ("text/sitemap", 0, Qt::CaseInsensitive ) != -1 )
            in_object = true;
        else if ( tagword == "/object" && in_object )
        {
            // a topic entry closed. Add a tree item
            if ( entry.name.isEmpty() && entry.urls.isEmpty() )
            {
                qWarning ("EBook_CHMImpl::ParseAndFillTopicsTree: <object> tag is parsed, but both name and url are empty.");
            }
            else
            {
                // If the name is empty, use the URL as name
                if ( entry.name.isEmpty() )
                    entry.name = entry.urls[0].toString();

                if ( !root_indent_offset_set )
                {
                    root_indent_offset_set = true;
                    root_indent_offset = indent;

                    if ( root_indent_offset > 1 )
                        qWarning("CHM has improper index; root indent offset is %d", root_indent_offset);
                }

                // Trim the entry name
                entry.name = entry.name.trimmed();

                int real_indent = indent - root_indent_offset;

                entry.indent = real_indent;
                data.push_back( entry );
            }

            entry.name = QString();
            entry.urls.clear();
            entry.seealso.clear();
            in_object = false;
        }
        else if ( tagword == "param" && in_object )
        {
            // <param name="Name" value="First Page">
            int offset; // strlen("param ")
            QString name_pattern = "name=", value_pattern = "value=";
            QString pname, pvalue;

            if ( (offset = tag.indexOf (name_pattern, 0, Qt::CaseInsensitive )) == -1 )
                qFatal ("EBook_CHMImpl::ParseAndFillTopicsTree: bad <param> tag '%s': no name=\n", qPrintable( tag ));

            // offset+5 skips 'name='
            offset = findStringInQuotes (tag, offset + name_pattern.length(), pname, true, false);
            pname = pname.toLower();

            if ( (offset = tag.indexOf(value_pattern, offset, Qt::CaseInsensitive )) == -1 )
                qFatal ("EBook_CHMImpl::ParseAndFillTopicsTree: bad <param> tag '%s': no value=\n", qPrintable( tag ));

            // offset+6 skips 'value='
            findStringInQuotes (tag, offset + value_pattern.length(), pvalue, false, true);

            if ( pname == "name" || pname == "keyword" )
            {
                // Some help files contain duplicate names, where the second name is empty. Work it around by keeping the first one
                if ( !pvalue.isEmpty() )
                    entry.name = pvalue;
            }
            else if ( pname == "merge" )
            {
                // MERGE implementation is experimental
                //QUrl mergeurl = pathToUrl( pvalue );
                QUrl mergeurl = QUrl( pvalue );
                QString mergecontent;

                //if ( getFileContentAsString( mergecontent, mergeurl ) && !mergecontent.isEmpty() )
                if(true)
                {
                    mergecontent = QString(chm->objectData(mergeurl.toString()));
                    qWarning( "MERGE is used in index; the implementation is experimental. Please let me know if it works" );

                    // Merge the read value into the current parsed file.
                    // To save memory it is done in a kinda hacky way:
                    src = mergecontent + src.mid( i );
                    pos = 0;
                    stringlen = src.length();
                }
                else
                    qWarning( "MERGE is used in index but file %s was not found in CHM archive", qPrintable(pvalue) );
            }
            else if ( pname == "local" )
            {
                // Check for URL duplication
                //QUrl url = pathToUrl( pvalue );
                QUrl url = QUrl( pvalue );

                if ( !entry.urls.contains( url ) )
                    entry.urls.push_back( url );
            }
            else if ( pname == "see also" && asIndex && entry.name != pvalue )
            {
                entry.urls.push_back( QUrl("seealso") );
                entry.seealso = pvalue;
            }
        }
        else if ( tagword == "ul" ) // increase indent level
        {
            // Fix for buggy help files
            if ( ++indent >= MAX_NEST_DEPTH )
                qFatal("EBook_CHMImpl::ParseAndFillTopicsTree: max nest depth (%d) is reached, error in help file", MAX_NEST_DEPTH);
        }
        else if ( tagword == "/ul" ) // decrease indent level
        {
            if ( --indent < root_indent_offset )
                indent = root_indent_offset;
        }

        pos = i;
    }

    return true;
}

int HhParser::findStringInQuotes(const QString& tag, int offset, QString& value, bool firstquote, bool decodeentities) const
{
    int qbegin = tag.indexOf ('"', offset);

    if ( qbegin == -1 )
        qFatal ("EBook_CHMImpl::findStringInQuotes: cannot find first quote in <param> tag: '%s'", qPrintable( tag ));

    int qend = firstquote ? tag.indexOf ('"', qbegin + 1) : tag.lastIndexOf ('"');

    if ( qend == -1 || qend <= qbegin )
        qFatal ("EBook_CHMImpl::findStringInQuotes: cannot find last quote in <param> tag: '%s'", qPrintable( tag ));

    // If we do not need to decode HTML entities, just return.
    if ( decodeentities )
    {
        QString htmlentity = QString();
        bool fill_entity = false;

        value.reserve (qend - qbegin); // to avoid multiple memory allocations

        for ( int i = qbegin + 1; i < qend; i++ )
        {
            if ( !fill_entity )
            {
                if ( tag[i] == '&' ) // HTML entity starts
                    fill_entity = true;
                else
                    value.append (tag[i]);
            }
            else
            {
                if ( tag[i] == ';' ) // HTML entity ends
                {
                    // If entity is an ASCII code, just decode it
                    QString decode = m_htmlEntityDecoder.decode( htmlentity );

                    if ( decode.isNull() )
                        break;

                    value.append ( decode );
                    htmlentity = QString();
                    fill_entity = false;
                }
                else
                    htmlentity.append (tag[i]);
            }
        }
    }
    else
        value = tag.mid (qbegin + 1, qend - qbegin - 1);

    return qend + 1;
}


ParsedEntry::ParsedEntry()
{
    indent = 0;
}
