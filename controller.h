#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

using str = QString;
using byteArr = QByteArray;


class COMWrapper_QSP: public QObject
{
    Q_OBJECT

    struct Settings
    {
        QString name;
        qint32 baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };


    Settings settings;

    byteArr m_writeData;
    qint64 m_bytesWritten;

protected:
    QSerialPort port;
    QTimer m_timer;
    str last_received_message;

public:
    explicit COMWrapper_QSP(QObject *parent = nullptr);

    virtual ~COMWrapper_QSP();

    void setPortSettings(str _name = str("COM1"), int _baudrate = 115200,
                         int _DataBits = 8, int _Parity = 0, int _StopBits = 1, int _FlowControl = 0);


signals:
    void finished_Port(); //Сигнал закрытия класса
    void error_(const str& err);//Сигнал ошибок порта
    void outPort(const str& data); //Сигнал вывода полученных данных

public slots:
    void DisconnectPort(); // Слот отключения порта
    void ConnectPort(); // Слот подключения порта
    void process_Port(); //Тело
    void WriteToPort(const byteArr& data); // Слот отправки данных в порт
    void WriteToPort(const str& str);

private slots:
    void handleError(QSerialPort::SerialPortError error);//Слот обработки ощибок
    void ReadInPort(); //Слот чтения из порта по ReadyRead

};

class PortController : public COMWrapper_QSP
{
    Q_OBJECT

public:
    explicit PortController();

public slots:
    bool isConnected();
    str FromComputer();
    str FromJoystick();
    str MoveXBy(int steps);
    str MoveYBy(int steps);
    str MoveZBy(int steps);
  //  str GetCoords(int* x, int* y, int* z);
    str MoveFrameLeft();
    str MoveFrameRight();
    str MoveFrameUp();
    str MoveFrameDown();


private:
    bool waitForAnswer(const str&);
    void sendCommandToPort(const str&);
    str  answerStatus(const str&);

};


#endif // CONTROLLER_H
