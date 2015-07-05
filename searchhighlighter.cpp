/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief cutecom-ng search result highliter implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "searchhighlighter.h"

SearchHighlighter::SearchHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
}

void SearchHighlighter::setSearchString(const QString &search)
{
    _search_string = search;
    rehighlight();
}


void SearchHighlighter::highlightBlock(const QString &text)
{
    // highlighted text background color (search results)
    static const Qt::GlobalColor HIGHLIGHT_COLOR = Qt::yellow;

    if (!_search_string.isEmpty() && !text.isEmpty())
    {
        QTextCharFormat charFormat;
        charFormat.setBackground(HIGHLIGHT_COLOR);

        const int length = _search_string.length();
        int index = text.indexOf(_search_string, 0, Qt::CaseInsensitive);
        while (index >= 0)
        {
            setFormat(index, length, charFormat);
            index = text.indexOf(_search_string, index + length, Qt::CaseInsensitive);
        }
    }
}
