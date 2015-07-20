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
 *      transfer should take place. At this point 'buffer' and
 *      'total_size' have already been set
 *
 *  - void cancelTransfer() : cancel the current transfer, this method
 *      can be called at any moment during the transfer
 *
 *  Furthermore, child class implementation should emit transferProgressed()
 *  signal so that the application can periodically inform the user about
 *  the transfer progression. transferProgressed() expects an integer
 *  representing current transfer percentage
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
        /// no synchronization
        NoSyncError              = 1,
        /// transmission error
        TransmissionError        = 2,
        /// transfer timed out
        TimeoutError             = 3,
        /// cancelled by remote
        RemoteCancelledError     = 4,
        /// cancelled locally
        LocalCancelledError      = 5,
        /// source file error
        InputFileError           = 6,
        /// unknown error
        UnknownError             = 7
    };

protected:

    /// file to transfer
    QString      filename;

    /// serial port instance to use for the transfer
    QSerialPort *serial;

    /// full content of file to transfer
    QByteArray   buffer;

    /// transferred file total size
    qint64       total_size;

    /// current transfer progress in percent
    int          cur_progress;

    /// worker thread in which transfer is processed
    QThread     *thread;

public:

    /**
     * \brief define timeout value for first serial port response
     * \param ms miliseconds elapsed before issuing a timeout
     *           if ms is -1 then the transfer will wait indefnitely
     */
    void setTimeout(int ms = -1);

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
     * \return error string
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
