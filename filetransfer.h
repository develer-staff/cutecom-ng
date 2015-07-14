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

    /// buffer to transfer
    QByteArray   buffer;

    /// transferred file total size
    qint64       total_size;

    /// current transfer progress in percent
    int          cur_progress;

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
     * \brief cancel current transfer
     */
    virtual void cancelTransfer() = 0;

    /**
     * \brief ask for cur_progress to be updated
     */
    virtual void updateProgress() = 0;

    /**
     * \brief return a string corresponding to given TransferError
     * \param error error code
     * \return error string
     */
    static QString errorString(TransferError error);

protected:
    /**
     * \brief TransferThread constructor
     * \param parent   object taking ownership
     * \param serial   opened instance of QSerialPort
     * \param filename file to transfer
     */
    FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename);

    /**
     * \brief set amount of bytes that have been set
     * \param sent_bytes total bytes sent since transfer start
     */
    void setSentBytes(int sent_bytes);

private:

    /**
     * \brief perform the actual file transfer
     * \return transfer end code
     */
    virtual TransferError performTransfer() = 0;

signals:
    /**
     * \brief signal emitted when file transfer has ended
     * \param code transfer end error code
     */
    void transferEnded(TransferError code);

    /**
     * \brief signal emitted each time tha file transfer progresses
     * (minimum amount to emit is 1% progress)
     * \percent percentage of file transfered
     */
    void transferProgressed(int percent);
};

#endif // TRANSFERTHREAD_H
