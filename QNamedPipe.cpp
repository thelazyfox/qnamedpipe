#include "QNamedPipe.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>

QNamedPipe::QNamedPipe(QString filename, QObject *parent) : QIODevice(parent), m_name(filename), m_fd(-1)
{
}

QNamedPipe::~QNamedPipe()
{   
}

bool QNamedPipe::open(OpenMode mode)
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

void QNamedPipe::close()
{
    if(isOpen()) {
        ::close(m_fd);
        m_fd = -1;
        QIODevice::close();
    }
}

qint64 QNamedPipe::bytesAvailable() const
{
    if(isOpen()) {
        int byteCount;
        fd_set fds;
        struct timeval timeout;
        
        FD_ZERO(&fds);
        FD_SET(m_fd, &fds);
        timeout.tv_usec = 0;
        timeout.tv_sec = 1;
        
        int n = select(m_fd + 1, &fds, 0, 0, &timeout);
        
        if(n == 1 && !ioctl(m_fd, FIONREAD, &byteCount))
            return byteCount + QIODevice::bytesAvailable();
    }
    return 0;
}

qint64 QNamedPipe::readData(char *data, qint64 maxSize)
{
    if(isOpen()) {
        int ret = ::read(m_fd, data, maxSize);
        if(ret >= 0) return ret;
    }
    return -1;
}

qint64 QNamedPipe::writeData(const char *data, qint64 maxSize)
{
    if(isOpen()) {
        int ret = ::write(m_fd, data, maxSize);
        if(ret >= 0) return ret;
    }
    return -1;
}
