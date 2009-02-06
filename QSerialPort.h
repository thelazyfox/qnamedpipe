#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

/* Currenty hard-wired for ReadWrite at 38400 */

#include <QIODevice>
#include <QString>

class QSerialPort : public QIODevice
{
public:
    QSerialPort(QString filename, QObject *parent = 0);
    ~QSerialPort();
    
    bool open(OpenMode mode = 0);
    void close();

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);
    
private:
    QString m_name;
    int m_fd;
};
#endif
