/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief cutecom-ng search result highliter header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef SEARCHHIGHLIGHTER_H
#define SEARCHHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

/**
 * \brief a syntax highliter used to highlight search results
 */
class SearchHighlighter : public QSyntaxHighlighter
{
private:
    /// the current search string
    QString _search_string;

    /// count occurences of search string in document
    int _num_occurences;

    /// current occurence of search string in document
    int _cur_occurence;

public:
    SearchHighlighter(QTextDocument *parent);

    /**
     * \brief define the search string     */
    void setSearchString(const QString &search);

    /**
     * \brief check for text to highlight
     * \param text text to check for the search string
     */
    void highlightBlock(const QString &text);

    /**
     * \brief highlist previous occurence of search string
     */
    void previousOccurence();

    /**
     * \brief highlist next occurence of search string
     */
    void nextOccurence();
};

#endif // SEARCHHIGHLIGHTER_H
