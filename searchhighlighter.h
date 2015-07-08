/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief cutecom-ng search result highlighter header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef SEARCHHIGHLIGHTER_H
#define SEARCHHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

/**
 * \brief syntax highlighter for search results
 *
 * - highlight the found occurences of a given string
 *  - manage an occurence cursor and its position,
 *    allowing to highlight one particular occurence
 */
class SearchHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:
    /// the current search string
    QString _search_string;

    /// indicate if highlighter maintains an occurence cursor
    bool has_cursor;

    /// count number N of occurences of search string in document
    int _num_occurences;

    /// occurence cursor [0, N-1]
    int _occurence_cursor;

    /// used to record position of occurences (in the whole document)
    /// through multiples highlightBlock calls
    int occurence_pos;

    /// last occurence cursor position
    int last_cursor_pos;

    /// indicates that search string has just been changed
    bool search_string_changed;

signals:
    /**
     * \brief signal emitted when current string changed
     * \param pos position of string
     */
    void cursorPosChanged(int pos);

    /**
     * \brief signal emitted when total number of search string occurences
     * has chnaged
     * \param total_occurences number of occurences:
     *     * -1: no search string
     *     *  0: search string defined but 0 occurences found
     *     *  n: n occurences of search string
     */
    void totalOccurencesChanged(int total_occurences);

public:
    /**
     * \brief SearchHighlighter construct a search results highlighter
     * \param parent parent widget
     * \param has_cursor_ maintain an occurence cursor
     */
    SearchHighlighter(QTextDocument *parent, bool has_cursor_ = true);

    /**
     * \brief define the search string     */
    void setSearchString(const QString &search);

    /**
     * \brief highlight given text block
     * \param text text in which looking for text to highlight
     */
    void highlightBlock(const QString &text);

    /**
     * \brief highlight previous occurence of search string
     */
    void previousOccurence();

    /**
     * \brief highlight next occurence of search string
     */
    void nextOccurence();

    /**
     * \brief get index of cursor occurence
     * \return 0 based index of cursor occurence
     */
    int cursorOccurence() const;

    /**
     * \brief get total number of search result occurences
     * \return total number of occurences
     */
    int totalOccurences() const;
};

#endif // SEARCHHIGHLIGHTER_H
