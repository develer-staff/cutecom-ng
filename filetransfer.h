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
class QTimer;

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
 *  - void updateProgress() : this method serves the purpose of updating
 *      a progress indicator for the user. updateProgress implementation
 *      must call FileTransfer::setSentBytes with the amount of bytes
 *      already transferred
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

    QTimer      *progress_timer;
    QThread     *thread;

public:

    /**
     * \brief file transfer destructor
     */
    ~FileTransfer();

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
     * \brief FileTransfer constructor
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
    virtual void performTransfer() = 0;

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

#endif // FILETRANSFER_H
