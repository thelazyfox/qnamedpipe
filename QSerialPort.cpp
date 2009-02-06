#include "QSerialPort.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

QSerialPort::QSerialPort(QString filename, QObject *parent) : QIODevice(parent), m_name(filename), m_fd(-1)
{
}

QSerialPort::~QSerialPort()
{   
}

bool QSerialPort::open(OpenMode mode)
{
    if(isOpen())
        return true;
    else if(mode == QIODevice::ReadOnly)
        m_fd = ::open(m_name.toLocal8Bit(), O_RDONLY);
    else if(mode == QIODevice::WriteOnly)
        m_fd = ::open(m_name.toLocal8Bit(), O_WRONLY);
    
    if(m_fd > 0) {
        ::fcntl(m_fd, F_SETFL, ~O_NDELAY & ::fcntl(m_fd, F_GETFL, 0));
        return QIODevice::open(ReadWrite);
    }
    
    return false;
}

void QSerialPort::close()
{
    if(m_fd > 0) {
        ::close(m_fd);
        m_fd = -1;
        QIODevice::close();
    }
}

qint64 QSerialPort::readData(char *data, qint64 maxSize)
{
    if(m_fd > 0) {
        int ret = ::read(m_fd, data, maxSize);
        if(ret >= 0) return ret;
    }
    return -1;
}

qint64 QSerialPort::writeData(const char *data, qint64 maxSize)
{
    if(m_fd > 0) {
        int ret = ::write(m_fd, data, maxSize);
        if(ret >= 0) return ret;
    }
    return -1;
}
