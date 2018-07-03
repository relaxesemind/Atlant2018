#include "portcontroller.h"

#include <QDebug>

int const delayMIN_for_answer = 150;
int const delayMAX_for_answer = 3000;
int const bytesDelay = 4000;
str const OK = str("OK");

COMWrapper_QSP::COMWrapper_QSP(QObject *parent)
{
    Q_UNUSED(parent);
    port.setPortName("COM1");
    port.setBaudRate(QSerialPort::Baud115200);
    port.setParity(QSerialPort::NoParity);
    port.setDataBits(QSerialPort::Data8);
    port.setStopBits(QSerialPort::OneStop);
}

COMWrapper_QSP::~COMWrapper_QSP()
{
    qDebug() << "port was closed";
    disconnectPort();
}

void COMWrapper_QSP::disconnectPort()
{
    if (port.isOpen())
    {
        port.close();
        emit finished_Port();
    }
}

void COMWrapper_QSP::connectPort()
{
    if (!port.isOpen())
    {
        port.open(QIODevice::ReadWrite);
        qDebug() << "port was open!";
    }
}

void COMWrapper_QSP::process_Port()
{
    qDebug() << "port is created";
    connect(&port,&QSerialPort::errorOccurred,
            this,&COMWrapper_QSP::handleError);

    connect(&port,&QSerialPort::readyRead,
            this,&COMWrapper_QSP::readInPort);
}

void COMWrapper_QSP::writeToPort(const QString& data)
{
    if (port.isOpen() and port.isWritable())
    {
        std::string data_s = data.toStdString();
        qint64 const bytesWritten = port.write(data_s.c_str());
        port.waitForBytesWritten(bytesDelay);

        if (bytesWritten == -1)
        {
            str err = QObject::tr("Failed to write the data to port %1, error: %2")
                                     .arg(port.portName())
                                     .arg(port.errorString());
            emit error_(err);
        }else if (bytesWritten != data.toLocal8Bit().size())
        {
            str err = QObject::tr("Failed to write all the data to port %1, error: %2")
                                     .arg(port.portName())
                                     .arg(port.errorString());
            emit error_(err);
        }
    }
}

void COMWrapper_QSP::handleError(QSerialPort::SerialPortError error)
{
    Q_UNUSED(error);
       emit error_(port.errorString().toLocal8Bit());
       disconnectPort();
}

void COMWrapper_QSP::readInPort()
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
    connectPort();
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

str PortController::fromComputer()
{
    sendCommandToPort(str("MTZ\r"));

    return str(answerStatus(OK));
}

str PortController::fromJoystick()
{
    sendCommandToPort("MTH\r");
    return str("Command FromJoystick was " + answerStatus(OK));
}

str PortController::moveXBy(int steps)
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

str PortController::moveYBy(int steps)
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

str PortController::moveZBy(int steps)
{

    sendCommandToPort(str("MTV=" + str::number(steps) + "\r"));
    waitForAnswer(OK);
    return str("Command MoveZBy " + str::number(steps) + " steps was " + answerStatus(str("Z_DONE")));
}

std::tuple<int, int, int> PortController::getCoordinates()
{
    //WriteCmd("MTGP\r");
    sendCommandToPort(str("MTGP\r"));
//    string S=WaitFor("_Y=");

//    int p1=S.find("_X=")+3;//search(S.begin(),S.end(),_x.begin(),_x.end())-S.begin()+sizeof(_x);// S.AnsiPos("_X=")+3;
//    int p2=S.find("_Y=")+3;//search(S.begin(),S.end(),_y.begin(),_y.end())-S.begin()+sizeof(_y);// S.AnsiPos("_Y=")+3;
//    int p3=S.find("_Z=")+3;//search(S.begin(),S.end(),_z.begin(),_z.end())-S.begin()+sizeof(_z);// S.AnsiPos("_Z=")+3;
//    string XS=S.substr(p1,p2-p1-5);
//    string YS;
//    string ZS;
//    if(p3>3)
//    {
//        YS=S.substr(p2,p3-p2-5);
//        ZS=S.substr(p3,S.length()-p3-1);
//    } else
//    {
//        YS=S.substr(p2,S.length()-p2-1);
//    }
//    if(x) *x=fromStr<int>(XS);//XS.ToInt();
//    if(y) *y=fromStr<int>(YS);//YS.ToInt();
//    if(p3>3)
//        if(z) *z=fromStr<int>(ZS); //ZS.ToInt();
//    return S;
//    //
//    waitForAnswer(str("_Y"));
    str answer = last_received_message;
    QStringList parse = answer.split("=");
    if (parse.size() == 3)
        return std::make_tuple(parse.at(0).toInt(),
                               parse.at(1).toInt(),
                               parse.at(2).toInt());
    else return std::make_tuple(1u-2,1u-2,1u-2);
}

str PortController::moveFrameLeft()
{
    sendCommandToPort(str("MTFL\r"));
    return str("Command MoveFrameLeft was " + answerStatus("X_DONE"));
}

str PortController::moveFrameRight()
{
    sendCommandToPort(str("MTFR\r"));
    return str("Command MoveFrameRight was " + answerStatus("X_DONE"));
}

str PortController::moveFrameUp()
{
    sendCommandToPort(str("MTFU\r"));
    return str("Command MoveFrameRight was " + answerStatus("Y_DONE"));
}

str PortController::moveFrameDown()
{
    sendCommandToPort(str("MTFD\r"));
    return str("Command MoveFrameRight was " + answerStatus("Y_DONE"));
}

str PortController::setSlowSpeed(int speed)
{
    sendCommandToPort(str("MTSII=") + str::number(speed) + str("\r"));
        //WriteCmd(string("MTSSI=")+IntToStr(speed)+"\r");
   return str("Command MoveFrameRight was " + answerStatus("SSI OK"));
}

str PortController::setFastSpeed(int speed)
{
   sendCommandToPort(str("MTSFI=") + str::number(speed) + str("\r"));

   return str("Command MoveFrameRight was " + answerStatus("SFI OK"));
}

str PortController::setZSpeed(int speed)
{

    sendCommandToPort(str("MTSFIZ=") + str::number(speed) + str("\r"));

    return str("Command MoveFrameRight was " + answerStatus("SFIzOK"));
}

str PortController::setBackX(int value)
{
    sendCommandToPort(str("MTSBX=") + str::number(value) + str("\r"));

    return str("Command MoveFrameRight was " + answerStatus("SBX OK"));
}

str PortController::setBackY(int value)
{
    sendCommandToPort(str("MTSBY=") + str::number(value) + str("\r"));

    return str("Command MoveFrameRight was " + answerStatus("SBY OK"));
}

str PortController::moveLeftStart(int speed)
{
    setSlowSpeed(speed);

    sendCommandToPort(str("MTL\r"));

    return str("Command MoveFrameRight was " + answerStatus("LEFT"));
}

str PortController::moveRightStart(int speed)
{
    setSlowSpeed(speed);
    sendCommandToPort(str("MTR\r"));

    return str("Command MoveFrameRight was " + answerStatus("RIGHT"));
}

str PortController::moveXStop()
{
    sendCommandToPort(str("MTSX\r"));

    return str("Command MoveFrameRight was " + answerStatus("STOP_X"));
}

str PortController::moveUpStart(int speed)
{
    setSlowSpeed(speed);
    sendCommandToPort(str("MTU\r"));

    return str("Command MoveFrameRight was " + answerStatus("UP"));
}

str PortController::moveDownStart(int speed)
{
    setSlowSpeed(speed);
    sendCommandToPort(str("MTD\r"));

    return str("Command MoveFrameRight was " + answerStatus("DOWN"));
}

str PortController::moveYStop()
{
    sendCommandToPort(str("MTSY\r"));

    return str("Command MoveFrameRight was " + answerStatus("STOP_Y"));
}

str PortController::setFrameSizeX(int value)
{
    sendCommandToPort(str("MTSFX=") + str::number(value) + str("\r"));

    return str("Command MoveFrameRight was " + answerStatus("SFX OK"));
}

str PortController::setFrameSizeY(int value)
{
    sendCommandToPort(str("MTSFY=") + str::number(value) + str("\r"));

    return str("Command MoveFrameRight was " + answerStatus("SFY OK"));
}


bool PortController::waitForAnswer(const str& ans)
{// wait outport signal or timeout
    if (!port.isReadable())
        return false;

    QEventLoop loop;
    connect(this,&PortController::outPort,&loop,&QEventLoop::quit);
    QTimer::singleShot(delayMAX_for_answer,&loop,&QEventLoop::quit);
    loop.exec();

    return last_received_message == ans;
}

void PortController::sendCommandToPort(const str& command)
{
    if (!port.isWritable())
        return;

    writeToPort(command);
}

str PortController::answerStatus(const str& ans)
{
    if (waitForAnswer(ans))
         return str("delivered");
    else return str("failed");
}
