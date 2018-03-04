#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSettings>

class COM_Control : public QObject
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

    QSerialPort port;
    Settings settings;
    bool isCorrectSettings;

    QByteArray m_writeData;
    qint64 m_bytesWritten = 0;
    QTimer m_timer;

public:
//    if(!BuildCommDCB("baud=115200 parity=N data=8 stop=1 xon=on",&dcb))
//        MessageBox(NULL,"!!!","!!!",MB_OK);
    explicit COM_Control(QString _name = QString("COM1"), int _baudrate = 115200,
                         int _DataBits = 8, int _Parity = 0, int _StopBits = 1, int _FlowControl = 0);
    ~COM_Control();

   QSerialPort& getPort();

signals:
    void finished_Port(); //Сигнал закрытия класса
    void error_(const QString& err);//Сигнал ошибок порта
    void outPort(const QString& data); //Сигнал вывода полученных данных

public slots:
    void DisconnectPort(); // Слот отключения порта
    void ConnectPort(void); // Слот подключения порта
    void process_Port(); //Тело
    void WriteToPort(const QByteArray &data); // Слот отправки данных в порт
    void WriteToPort(const QString& str);

 private slots:
     void handleError(QSerialPort::SerialPortError error);//Слот обработки ощибок
     void ReadInPort(); //Слот чтения из порта по ReadyRead
     void handleTimeout();
     void handleBytesWritten(qint64);
};


/**
class StageControl: public COMWrapper
{
public:
    char WaitForChar(char c)
    {
        char s;
        do
            s=ReadOne();
        while (s!=c);
        return s;
    }
    string WaitFor(const string& what)
    {
        string S;
        do
        {
            S+=Read();
            if(S.find(what)!=string::npos)//if(!((S.AnsiPos(what)==0)||(S[S.Length()]!='\n')))
            {
                break;
            }
        }
        while(true);
        return S;
    }
    string WaitForAll(const string* what, int count)
    {
        list<string> what_l(what,what+count);
        string S;
        do
        {
            S+=Read();
            for(list<string>::iterator i=what_l.begin(); i!=what_l.end(); ++i)
            {
                if(S.find(*i)!=string::npos)//if(S.AnsiPos(*i)>0)
                {
                    i=what_l.erase(i);
                    --i;
                }
            }
        }
        while((!what_l.empty())||(S[S.length()]!='\n'));
        return S;
    }
    string WaitForOne(const string* what, int count)
    {
        list<string> what_l(what,what+count);
        string S;
        do
        {
            S+=Read();
            for(list<string>::iterator i=what_l.begin(); i!=what_l.end(); ++i)
            {
                if(S.find(*i)!=string::npos) { return *i; }//if(S.AnsiPos(*i)>0) { return *i; }
            }
        }
        while(true);
    }
    void WriteCmd(const string& cmd)
    {
        DWORD bw;
        if(delay<0)
        {
            WriteFile(Handle, cmd.c_str(), cmd.length(), &bw, NULL);
            if(bw!=cmd.length()) MessageBox(NULL,"!!","!!",MB_OK);
        } else
            for(unsigned int i=0; i<cmd.length(); ++i)
            {
                //  WriteFile(Handle, cmd.c_str(), cmd.length(), &bw, NULL);
                char c=cmd[i];
                WriteFile(Handle, &c, 1, &bw, NULL);
                if(bw!=1) MessageBox(NULL,"!!","!!",MB_OK);
                Sleep(delay);
                //                  WaitForChar(c);
            }
    }
    StageControl(const string& port, int char_delay): COMWrapper(port, char_delay)
    {
        FromComputer();
    }
    string FromComputer()
    {
        WriteCmd("MTZ\r");
        return WaitFor("OK");
    }
    string FromJoystick()
    {
        WriteCmd("MTH\r");
        return WaitFor("OK");
    }
    string MoveXBy(int steps)
    {
        std::stringstream ss;
        ss<<"MTX="<<steps<<"\r";
        WriteCmd(ss.str());
        // WriteCmd(string("MTX=")+IntToStr(steps)+"\r");
        WaitFor("OK");
        return WaitFor("X_DONE");
    }
    string MoveYBy(int steps)
    {
        std::stringstream ss;
        ss<<"MTY="<<steps<<"\r";
        WriteCmd(ss.str());
        //WriteCmd(string("MTY=")+IntToStr(steps)+"\r");
        WaitFor("OK");
        return WaitFor("Y_DONE");
    }
    string MoveXYBy(int steps_x, int steps_y)
    {
        std::stringstream ss1;
        ss1<<"MTX="<<steps_x<<"\r";
        WriteCmd(ss1.str());
        // WriteCmd(string("MTX=")+IntToStr(steps_x)+"\r");
        WaitFor("OK");

        std::stringstream ss2;
        ss2<<"MTY="<<steps_y<<"\r";
        WriteCmd(ss2.str());
        //  WriteCmd(string("MTY=")+IntToStr(steps_y)+"\r");
        WaitFor("OK");

        string arr[2]={"X_DONE","Y_DONE"};
        return WaitForAll(arr,2);
    }
    string MoveZBy(int steps)
    {
        std::stringstream ss;
        ss<<"MTV="<<steps<<"\r";
        WriteCmd(ss.str());
        // WriteCmd(string("MTV=")+IntToStr(steps)+"\r");
        WaitFor("OK");
        return WaitFor("Z_DONE");
    }

    string GetCoords(int* x, int* y, int* z)
    {
        WriteCmd("MTGP\r");
        //const string _x("_X=");
        // const string _y("_Y=");
        // const string _z("_Z=");
        string S=WaitFor("_Y=");

        int p1=S.find("_X=")+3;//search(S.begin(),S.end(),_x.begin(),_x.end())-S.begin()+sizeof(_x);// S.AnsiPos("_X=")+3;
        int p2=S.find("_Y=")+3;//search(S.begin(),S.end(),_y.begin(),_y.end())-S.begin()+sizeof(_y);// S.AnsiPos("_Y=")+3;
        int p3=S.find("_Z=")+3;//search(S.begin(),S.end(),_z.begin(),_z.end())-S.begin()+sizeof(_z);// S.AnsiPos("_Z=")+3;
        string XS=S.substr(p1,p2-p1-5);
        string YS;
        string ZS;
        if(p3>3)
        {
            YS=S.substr(p2,p3-p2-5);
            ZS=S.substr(p3,S.length()-p3-1);
        } else
        {
            YS=S.substr(p2,S.length()-p2-1);
        }
        if(x) *x=fromStr<int>(XS);//XS.ToInt();
        if(y) *y=fromStr<int>(YS);//YS.ToInt();
        if(p3>3)
            if(z) *z=fromStr<int>(ZS); //ZS.ToInt();
        return S;
    }
    string SetSlowSpeed(int speed)
    {
        std::stringstream ss;
        ss<<"MTSSI="<<speed<<"\r";
        WriteCmd(ss.str());

        //WriteCmd(string("MTSSI=")+IntToStr(speed)+"\r");
        return WaitFor("SSI OK");
    }
    string SetFastSpeed(int speed)
    {
        std::stringstream ss;
        ss<<"MTSFI="<<speed<<"\r";
        WriteCmd(ss.str());

        //  WriteCmd(string("MTSFI=")+IntToStr(speed)+"\r");
        return WaitFor("SFI OK");
    }
    string SetZSpeed(int speed)
    {
        std::stringstream ss;
        ss<<"MTSFIZ="<<speed<<"\r";
        WriteCmd(ss.str());

        //WriteCmd(string("MTSFIZ=")+IntToStr(speed)+"\r");
        return WaitFor("SFIzOK");
    }
    string SetBackX(int value)
    {
        std::stringstream ss;
        ss<<"MTSBX="<<value<<"\r";
        WriteCmd(ss.str());
        // WriteCmd(string("MTSBX=")+IntToStr(value)+"\r");
        return WaitFor("SBX OK");
    }
    string SetBackY(int value)
    {
        std::stringstream ss;
        ss<<"MTSBY="<<value<<"\r";
        WriteCmd(ss.str());
        // WriteCmd(string("MTSBY=")+ IntToStr(value)+"\r");
        return WaitFor("SBY OK");
    }
    string MoveLeftStart(int speed)
    {
        SetSlowSpeed(speed);
        WriteCmd("MTL\r");
        return WaitFor("LEFT");
    }
    string MoveRightStart(int speed)
    {
        SetSlowSpeed(speed);
        WriteCmd("MTR\r");
        return WaitFor("RIGHT");
    }
    string MoveXStop()
    {
        WriteCmd("MTSX\r");
        return WaitFor("STOP_X");
    }
    string MoveUpStart(int speed)
    {
        SetSlowSpeed(speed);
        WriteCmd("MTU\r");
        return WaitFor("UP");
    }
    string MoveDownStart(int speed)
    {
        SetSlowSpeed(speed);
        WriteCmd("MTD\r");
        return WaitFor("DOWN");
    }
    string MoveYStop()
    {
        WriteCmd("MTSY\r");
        return WaitFor("STOP_Y");
    }
    string SetFrameSizeX(int Value)
    {
        std::stringstream ss;
        ss<<"MTSFX="<<Value<<"\r";
        WriteCmd(ss.str());
        //WriteCmd("MTSFX="+IntToStr(Value)+"\r");
        return WaitFor("SFX OK");
    }
    string SetFrameSizeY(int Value)
    {
        std::stringstream ss;
        ss<<"MTSFY="<<Value<<"\r";
        WriteCmd(ss.str());


        return WaitFor("SFY OK");
    }
    string MoveFrameLeft()
    {
        WriteCmd("MTFL\r");
        return WaitFor("X_DONE");
    }
    string MoveFrameRight()
    {
        WriteCmd("MTFR\r");
        return WaitFor("X_DONE");
    }
    string MoveFrameUp()
    {
        WriteCmd("MTFU\r");
        return WaitFor("Y_DONE");
    }
    string MoveFrameDown()
    {
        WriteCmd("MTFD\r");
        return WaitFor("Y_DONE");
    }
};

**/

#endif // PORT_H
