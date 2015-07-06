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
    QSyntaxHighlighter(parent),
    _num_occurences(0),
    _cur_occurence(0)
{
}

void SearchHighlighter::setSearchString(const QString &search)
{
    _search_string = search;
    _cur_occurence = 0;
    _num_occurences = 0;
    rehighlight();
}

#include <QDebug>
void SearchHighlighter::highlightBlock(const QString &text)
{
    // highlighted text background color (search results)
    static const Qt::GlobalColor SEARCH_RESULT_BACKGROUND_COLOR = Qt::yellow;

    // highlighted text background color (search results)
    static const Qt::GlobalColor CURRENT_SEARCH_RESULT_BACKGROUND_COLOR = Qt::red;

    if (!_search_string.isEmpty() && !text.isEmpty())
    {
        QTextCharFormat charFormat;
        charFormat.setBackground(SEARCH_RESULT_BACKGROUND_COLOR);

        const int length = _search_string.length();
        int index = text.indexOf(_search_string, 0, Qt::CaseInsensitive);
        while (index >= 0)
        {
            charFormat.setBackground(SEARCH_RESULT_BACKGROUND_COLOR);
            if (_num_occurences == _cur_occurence)
                charFormat.setBackground(CURRENT_SEARCH_RESULT_BACKGROUND_COLOR);

            setFormat(index, length, charFormat);
            index = text.indexOf(_search_string, index + length, Qt::CaseInsensitive);
            ++_num_occurences;
        }
    }

    qDebug() << "occurences: " << _num_occurences;

}

void SearchHighlighter::previousOccurence()
{
    // nothing to do if the string-search returned nothing
    if (_num_occurences > 0)
    {
        // cyclic behaviour
        if (_cur_occurence == 0)
            _cur_occurence = _num_occurences - 1;
        else
            --_cur_occurence;
        _num_occurences = 0;
        rehighlight();
    }
}

void SearchHighlighter::nextOccurence()
{
    // nothing to do if the string-search returned nothing
    if (_num_occurences > 0)
    {
        // cyclic behaviour
        if (_cur_occurence == _num_occurences - 1)
            _cur_occurence = 0;
        else
            ++_cur_occurence;
        _num_occurences = 0;
        rehighlight();
    }
}

