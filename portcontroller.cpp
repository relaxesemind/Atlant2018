#include "portcontroller.h"

#include <QDebug>

int const delayMIN_for_answer = 150;
int const delayMAX_for_answer = 3000;
int const bytesDelay = 4000;
str const OK = str("OK");



COMWrapper_QSP::COMWrapper_QSP(QObject *parent)
{
    Q_UNUSED(parent);

    m_timer.setSingleShot(true);
}

COMWrapper_QSP::~COMWrapper_QSP()
{
    qDebug() << "port was closed";
    DisconnectPort();
}

void COMWrapper_QSP::setPortSettings(str _name, int _baudrate, int _DataBits, int _Parity, int _StopBits, int _FlowControl)
{
    settings.name = _name;
    settings.baudRate = _baudrate;
    settings.dataBits = static_cast<QSerialPort::DataBits> (_DataBits);
    settings.parity = static_cast<QSerialPort::Parity> (_Parity);
    settings.stopBits = static_cast<QSerialPort::StopBits> (_StopBits);
    settings.flowControl = static_cast<QSerialPort::FlowControl> (_FlowControl);

    port.setPortName(settings.name);
    port.setBaudRate(settings.baudRate);
    port.setDataBits(settings.dataBits);
    port.setParity(settings.parity);
    port.setStopBits(settings.stopBits);
    port.setFlowControl(settings.flowControl);
}

void COMWrapper_QSP::DisconnectPort()
{
    if (port.isOpen())
    {
        port.close();
        emit finished_Port();
    }
}

void COMWrapper_QSP::ConnectPort()
{
    if (!port.isReadable() or !port.isWritable())
        return;

    if (port.open(QIODevice::ReadWrite))
    {

    }else{

    }
}

void COMWrapper_QSP::process_Port()
{
    qDebug() << "port is created";
    connect(&port,&QSerialPort::errorOccurred,this,&COMWrapper_QSP::handleError);
    connect(&port,&QSerialPort::readyRead,this,&COMWrapper_QSP::ReadInPort);
}

void COMWrapper_QSP::WriteToPort(const byteArr &data)
{
    if (port.isOpen() and port.isWritable())
    {
        m_writeData = data;
        qint64 const bytesWritten = port.write(data);
        port.waitForBytesWritten(bytesDelay);

        if (bytesWritten == -1)
        {
            str err = QObject::tr("Failed to write the data to port %1, error: %2")
                                     .arg(port.portName())
                                     .arg(port.errorString());
            emit error_(err);
        }else if (bytesWritten != m_writeData.size())
        {
            str err = QObject::tr("Failed to write all the data to port %1, error: %2")
                                     .arg(port.portName())
                                     .arg(port.errorString());
            emit error_(err);
        }
    }
}

void COMWrapper_QSP::WriteToPort(const str& str)
{
    byteArr bytearr(str.toLocal8Bit());
    WriteToPort(bytearr);
}

void COMWrapper_QSP::handleError(QSerialPort::SerialPortError error)
{
    Q_UNUSED(error);
       emit error_(port.errorString().toLocal8Bit());
       DisconnectPort();
}

void COMWrapper_QSP::ReadInPort()
{
    str data (port.readAll());
    if (!data.isEmpty())
    {
        last_received_message = data;
        outPort(data);
    }
}


PortController::PortController() : COMWrapper_QSP()
{
    setPortSettings(); //default settings

}

bool PortController::isConnected()
{
    return port.isOpen();
}

bool PortController::isWritable()
{
    return port.isWritable();
}

bool PortController::isReadable()
{
    return port.isReadable();
}

str PortController::FromComputer()
{
    sendCommandToPort(str("MTZ\r"));

    return str("Command FromComputer was " + answerStatus(OK));
}

str PortController::FromJoystick()
{
    sendCommandToPort("MTH\r");
    return str("Command FromJoystick was " + answerStatus(OK));
}

str PortController::MoveXBy(int steps)
{
//    std::stringstream ss;
//    ss<<"MTX="<<steps<<"\r";
//    WriteCmd(ss.str());
//    // WriteCmd(string("MTX=")+IntToStr(steps)+"\r");
//    WaitFor("OK");
//    return WaitFor("X_DONE");
    sendCommandToPort(str("MTX=" + str::number(steps) + "\r"));
    waitForAnswer(OK);
    return str("Command MoveXBy " + str::number(steps) + " steps was " + answerStatus(str("X_DONE")));
}

str PortController::MoveYBy(int steps)
{
//    std::stringstream ss;
//    ss<<"MTY="<<steps<<"\r";
//    WriteCmd(ss.str());
//    //WriteCmd(string("MTY=")+IntToStr(steps)+"\r");
//    WaitFor("OK");
//    return WaitFor("Y_DONE");
    sendCommandToPort(str("MTY=" + str::number(steps) + "\r"));
    waitForAnswer(OK);
    return str("Command MoveYBy " + str::number(steps) + " steps was " + answerStatus(str("Y_DONE")));
}

str PortController::MoveZBy(int steps)
{

    sendCommandToPort(str("MTV=" + str::number(steps) + "\r"));
    waitForAnswer(OK);
    return str("Command MoveZBy " + str::number(steps) + " steps was " + answerStatus(str("Z_DONE")));
}

str PortController::MoveFrameLeft()
{
    sendCommandToPort(str("MTFL\r"));
    return str("Command MoveFrameLeft was " + answerStatus("X_DONE"));
}

str PortController::MoveFrameRight()
{
    sendCommandToPort(str("MTFR\r"));
    return str("Command MoveFrameRight was " + answerStatus("X_DONE"));
}

str PortController::MoveFrameUp()
{
    sendCommandToPort(str("MTFU\r"));
    return str("Command MoveFrameRight was " + answerStatus("Y_DONE"));
}

str PortController::MoveFrameDown()
{
    sendCommandToPort(str("MTFD\r"));
    return str("Command MoveFrameRight was " + answerStatus("Y_DONE"));
}


bool PortController::waitForAnswer(const str& ans)
{
    if (!port.isReadable())
        return false;

    int n = 0;

    while ( !last_received_message.contains(ans,Qt::CaseInsensitive) and
           delayMIN_for_answer * (++n) <= delayMAX_for_answer )
    {
        QThread::currentThread()->msleep(delayMIN_for_answer);
    }
    return last_received_message == ans;
}

void PortController::sendCommandToPort(const str& command)
{
    if (!port.isWritable())
        return;

    WriteToPort(command);
}

str PortController::answerStatus(const str& ans)
{
    if (waitForAnswer(ans))
         return str("delivered");
    else return str("failed");
}
