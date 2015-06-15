/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief handle display and saving of serial port output
 *
 * \version $Id: $
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>

class OutputManager : public QObject
{
    Q_OBJECT
public:
    explicit OutputManager(QObject *parent = 0);

signals:

public slots:
};

#endif // OUTPUTMANAGER_H
