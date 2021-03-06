#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <string>
#include <QByteArray>
#include <QThread>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QEventLoop>
#include <tuple>
#include <QStringList>


using str = QString;
using byteArr = QByteArray;


class COMWrapper_QSP: public QObject
{
    Q_OBJECT

    qint64 m_bytesWritten;

protected:
    QSerialPort port;
    str last_received_message;

public:
    explicit COMWrapper_QSP(QObject *parent = nullptr);

    virtual ~COMWrapper_QSP();

signals:
    void finished_Port(); //Сигнал закрытия класса
    void error_(const str& err);//Сигнал ошибок порта
    void outPort(const str& data); //Сигнал вывода полученных данных

public slots:
    void disconnectPort(); // Слот отключения порта
    void connectPort(); // Слот подключения порта
    void process_Port(); //Тело
    void writeToPort(const QString&); // Слот отправки данных в порт

private slots:
    void handleError(QSerialPort::SerialPortError error);//Слот обработки ощибок
    void readInPort(); //Слот чтения из порта по ReadyRead

};

class PortController : public COMWrapper_QSP
{
    Q_OBJECT
public:
    explicit PortController();


public slots:
    bool isConnected();
    bool isWritable();
    bool isReadable();
    str fromComputer();
    str fromJoystick();
    str moveXBy(int steps);
    str moveYBy(int steps);
    str moveZBy(int steps);
    std::tuple<int,int,int> getCoordinates();
    str moveFrameLeft();
    str moveFrameRight();
    str moveFrameUp();
    str moveFrameDown();
    str setSlowSpeed(int speed);
    str setFastSpeed(int speed);
    str setZSpeed(int speed);
    str setBackX(int value);
    str setBackY(int value);
    str moveLeftStart(int speed);
    str moveRightStart(int speed);
    str moveXStop();
    str moveUpStart(int speed);
    str moveDownStart(int speed);
    str moveYStop();
    str setFrameSizeX(int value);
    str setFrameSizeY(int value);


private:
    bool waitForAnswer(const str&);
    void sendCommandToPort(const str&);
    str  answerStatus(const str&);
};


#endif // CONTROLLER_H
