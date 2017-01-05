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

    bool parse(const QString& file, QList<ParsedEntry>& data, bool asIndex ) const;

private:
    int findStringInQuotes(const QString& tag, int offset, QString& value, bool firstquote, bool decodeentities) const;

    ChmFile *chm;
    HelperEntityDecoder m_htmlEntityDecoder;
};

#endif // HHPARSER_H
