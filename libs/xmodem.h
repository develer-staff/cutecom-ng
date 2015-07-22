/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief xmodem header
 *
 *  this file is not part of Georges Menie xmodem implementation
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#ifndef XMODEM_H
#define XMODEM_H

/**
* \brief perform xmodem transmission over serial port
* \param src address of buffer to send over serial port
* \param srcsz transmit srcsz bytes of src buffer
* \param quit_asap if true, the transmission should end as
*   soon as possible
* \return error code
*/
int xmodemTransmit(unsigned char *src, int srcsz, volatile bool *quit_asap);

#endif // XMODEM_H

