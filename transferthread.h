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

#include <QThread>
#include <QMutex>

class QSerialPort;

/**
 * \brief file transfer worker thread
 */
class TransferThread : public QThread
{
    Q_OBJECT

//    Q_PROPERTY(TransferError error READ error RESET clearError NOTIFY error)

public:

    /**
     * \brief The TransferError enum
     */
    enum TransferError
    {
        /// transfer cancelled
        CancelledError = -1,
        /// file successfully transferred
        NoError = 0,
        /// transfer timed out
        TimeoutError = 1
    };

protected:

    /// file to transfer
    QString      filename;

    /// serial port instance to use for the transfer
    QSerialPort *serial;

    /// timeout
    int          wait_timeout;

    /// if true, indicates that the worker thread has to be stopped asap
    bool         quit;

public:

    //*** REFINE THIS METHOD ***//
    // TIMEOUT BEFORE WHAT??
    // - between start and end of transfer? no..
    // - before actual transfer (so would be handshaking timeout)
    // is this setTimeout corresponding to the waitTimeout member value??

    // maybe also define a number of retries

    /**
     * \brief define timeout
     * \param ms miliseconds elapsed before issuing a timeout
     *           if ms is -1 then the transfer will wait indefnitely
     */
    void setTimeout(int ms = -1);

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
    TransferThread(QObject *parent, QSerialPort *serial, const QString &filename);
    ~TransferThread();

    /**
     * \brief thread starting point
     */
    void run();

private:
    /**
     * \brief start the actual file transfer implemented in child class
     * \return transfer end code
     */
    virtual TransferError startTransfer() = 0;

signals:
    /**
     * \brief signal emitted when file transfer has ended
     * \param reason reason for transfer having ended
     */
    void transferEnded(TransferError reason);
};

#endif // TRANSFERTHREAD_H
