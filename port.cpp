#include "port.h"

QSerialPort& COM_Control::getPort()
{
    return port;
}

COM_Control::COM_Control(QString _name, int _baudrate, int _DataBits, int _Parity, int _StopBits, int _FlowControl)
{

    settings.name = _name;
    settings.baudRate = _baudrate;
    settings.dataBits = static_cast<QSerialPort::DataBits> (_DataBits);
    settings.parity = static_cast<QSerialPort::Parity> (_Parity);
    settings.stopBits = static_cast<QSerialPort::StopBits> (_StopBits);
    settings.flowControl = static_cast<QSerialPort::FlowControl> (_FlowControl);

    port.setPortName(settings.name);

    isCorrectSettings =
       port.setBaudRate(settings.baudRate)
    && port.setDataBits(settings.dataBits)
    && port.setParity(settings.parity)
    && port.setStopBits(settings.stopBits)
            && port.setFlowControl(settings.flowControl);
    m_timer.setSingleShot(true);
}

COM_Control::~COM_Control()
{
    qDebug("By in Thread!");
    emit finished_Port();//Сигнал о завершении работы
}

void COM_Control::DisconnectPort()
{
    if (port.isOpen())
    {
        port.close();
        error_(QString(settings.name.toLocal8Bit() + " >> is close!\r").toLocal8Bit());
    }
}

void COM_Control::ConnectPort()
{
    if (!port.isReadable() or !isCorrectSettings)
         return;

    if (port.open(QIODevice::ReadWrite))
        {
                error_((settings.name+ " >> is open!\r").toLocal8Bit());
        } else {
            port.close();
            error_(port.errorString().toLocal8Bit());
        }
}

void COM_Control::process_Port()
{
    qDebug("Hello World in Thread!");
    connect(&port,&QSerialPort::errorOccurred,this,&COM_Control::handleError);
    connect(&port,&QSerialPort::readyRead,this,&COM_Control::ReadInPort);
  //----------------------------------------------------------------------------


}

void COM_Control::WriteToPort(const QByteArray &data)
{
    if (port.isOpen())
    {
        m_writeData = data;
        qint64 const bytesWritten = port.write(data);
        if (bytesWritten == -1)
        {
            m_standartOutput << QObject::tr("Failed to write the data to port %1, error: %2")
                                .arg(port.portName())
                                .arg(port.errorString())
                             << endl;
        }else if (bytesWritten != m_writeData.size())
        {
            m_standartOutput << QObject::tr("Failed to write all the data to port %1, error: %2")
                                        .arg(port.portName())
                                        .arg(port.errorString())
                                     << endl;
        }

    }
    m_timer.start(1000);
}

void COM_Control::WriteToPort(const QString& str)
{
     QByteArray bytearr(str.toLocal8Bit());
     WriteToPort(bytearr);
}

void COM_Control::handleError(QSerialPort::SerialPortError error)
{
    if ( (port.isOpen()) && (error == QSerialPort::ResourceError)) {
        error_(port.errorString().toLocal8Bit());
        DisconnectPort();
    }
}

void COM_Control::ReadInPort()
{
    QByteArray data(port.readAll());
    outPort(data);
}

void COM_Control::handleTimeout()
{
    m_standartOutput << QObject::tr("Operation timed out for port %1, error: %2")
                        .arg(port.portName())
                        .arg(port.errorString())
                     << endl;
}

void COM_Control::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.size())
    {
        m_bytesWritten = 0;
        m_standartOutput << QObject::tr("Data successfully sent to port %1")
                            .arg(port.portName())
                         << endl;

    }
}










