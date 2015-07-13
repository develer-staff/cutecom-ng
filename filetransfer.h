/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief TransferThread class header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef TRANSFERTHREAD_H
#define TRANSFERTHREAD_H

#include <QObject>

class QSerialPort;

/**
 * \brief serial port file transfer
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
        /// unknown error
        UnknownError             = 6
    };

protected:

    /// file to transfer
    QString      filename;

    /// serial port instance to use for the transfer
    QSerialPort *serial;

    /// timeout
    int          wait_timeout;

    /// if true, indicates that the worker thread has to be stopped asap
    bool         should_quit;

    /// buffer to transfer
    QByteArray   buffer;

public:

    /**
     * \brief define timeout value for first serial port response
     * \param ms miliseconds elapsed before issuing a timeout
     *           if ms is -1 then the transfer will wait indefnitely
     */
    void setTimeout(int ms = -1);

    /**
     * \brief start the file transfer
     */
    void startTransfer();

    /**
     * \brief cancel pending transfer, this results in the emission
     * of the transferEnded signal(TransferError::CancelledError)
     */
    void cancelTransfer();

protected:
    /**
     * \brief TransferThread constructor
     * \param parent   object taking ownership
     * \param serial   opened instance of QSerialPort
     * \param filename file to transfer
     */
    FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename);
    ~FileTransfer();

private:

    /**
     * \brief perform the actual file transfer
     * \return transfer end code
     */
    virtual TransferError performTransfer() = 0;

signals:

    /**
     * \brief signal emitted when file transfer has ended
     * \param reason reason for transfer having ended
     */
    void transferError(TransferError reason);

    void transferFinished();
};

#endif // TRANSFERTHREAD_H
