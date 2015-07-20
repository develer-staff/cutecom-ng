/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief FileTransfer class header
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>

class QSerialPort;

/**
 * \brief serial port file transfer abstract class
 *
 * child classes implementing a specific file transfer protocol must
 * implement these pure virtual methods :
 *
 *  - TransferError performTransfer() : this is where the actual file
 *      transfer must take place. At this point 'buffer' and
 *      'total_size' have been set
 *
 *  - void cancelTransfer() : cancel the current transfer, this method
 *      can be called at any moment during the transfer
 *
 *  Furthermore, child class implementation must emit transferProgressed()
 *  signal frequently enough, so that the application can inform the user
 *  about the current state of the transfer. the integer parameter of
 *  transferProgressed() should represents the percentage of current
 *  transfer that has already been performed
 */
class FileTransfer : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief The TransferError enum
     */
    enum TransferError
    {
        /// file successfully transferred
        NoError                  = 0,
        /// no synchronization between local and remote
        NoSyncError              = 1,
        /// transmission error
        TransmissionError        = 2,
        /// transfer timed out
        TimeoutError             = 3,
        /// transfer cancelled by remote
        RemoteCancelledError     = 4,
        /// transfer cancelled locally
        LocalCancelledError      = 5,
        /// local file error (size, permission)
        InputFileError           = 6,
        /// unknown error
        UnknownError             = 7
    };

protected:

    /// file to transfer
    QString      filename;

    /// serial port instance used for the transfer
    QSerialPort *serial;

    /// full content of file to transfer
    QByteArray   buffer;

    /// total size in bytes of buffer
    qint64       total_size;

    /// thread in which the transfer is performed
    QThread     *thread;

public:

    /**
     * \brief start the file transfer
     * \return boolean indicating wether transfer has started or not
     */
    bool startTransfer();

    /**
     * \brief cancel current transfer
     */
    virtual void cancelTransfer() = 0;

    /**
     * \brief return a string corresponding to given TransferError
     * \param error error code
     * \return error error string
     */
    static QString errorString(TransferError error);

protected:
    /**
     * \brief FileTransfer constructor
     * \param parent   object taking ownership
     * \param serial   opened instance of QSerialPort
     * \param filename file to transfer
     */
    FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename);

private:

    /**
     * \brief perform the actual file transfer
     * \return transfer end code
     */
    virtual void performTransfer() = 0;

    /**
     * \brief handle transferEnded signal
     * \param error transfer end error code
     */
    void handleTransferEnded(TransferError error);

signals:
    /**
     * \brief signal emitted when file transfer has ended
     * \param error transfer end error code
     */
    void transferEnded(TransferError error);

    /**
     * \brief signal emitted each time the file transfer progresses
     * (minimum amount to emit is 1% progress)
     * \percent percentage of file transfered
     */
    void transferProgressed(int percent);
};

#endif // FILETRANSFER_H
