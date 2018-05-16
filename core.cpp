#include "core.h"
#include "cellsselecter.h"
#include <QMessageBox>


bool portisWorking = false;

Core::Core(QObject *parent) : QObject(parent)
{
    m_timer.setSingleShot(true);
  //  AutoFocusProcessManager::sharedManager().moveToThread(focusWorkThread);
  //  CellsSelecter::sharedSelecter().moveToThread(selecterThread);
    AutoFocusProcessManager::sharedManager().setCore(this);
//    connect(&AutoFocusProcessManager::sharedManager(),&AutoFocusProcessManager::focusingValueWasChanged,
//            this,&Core::updateFocusQualityBar,Qt::DirectConnection);
   // focusWorkThread->start(QThread::NormalPriority);
}

Core::~Core()
{
    if (camPtr)
    {
         camPtr->onStop();
    }
    if (runFocus)
    {
        runFocus->onStop();
    }
    cam_pool.waitForDone();
    autoFocus_threadPool.waitForDone();

}

void Core::makeConnetionWithPort()
{
    if (portisWorking == true)
        return;
    portisWorking = true;

    COM1port = std::make_unique<PortController> ();
    QThread* thread = new QThread();
    COM1port.get() -> moveToThread(thread);

    connect(thread,&QThread::started,COM1port.get(),&PortController::process_Port,Qt::DirectConnection);
    connect(COM1port.get(),&PortController::finished_Port,thread,&QThread::quit,Qt::DirectConnection);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater,Qt::DirectConnection);
    connect(COM1port.get(),&PortController::outPort,this,&Core::feedBackFromPort,Qt::DirectConnection);
    //
    thread->start();
}

void Core::disconnectPort()
{
    if (portisWorking == false)
        return;
    portisWorking = false;
    COM1port.reset(nullptr);
}

QString Core::moveToRight(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveXBy(steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToLeft(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveXBy(-steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToForward(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveYBy(steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToBackward(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveYBy(-steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToUp(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
        return COM1port->moveZBy(steps);
    }else
    {
        return QString("port error");
    }
}

QString Core::moveToDown(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
        return COM1port->moveZBy(-steps);
    }else
    {
        return QString("port error");
    }
}

QString Core::checkPortStatus()
{
    if (COM1port == nullptr)
    {
        return QString("port is null!");
    }
    QString status;
    if (COM1port->isConnected())
    {
        status = "port is connected!";
    }else{
        status = "port is disconnected!";
    }

    if (COM1port->isWritable())
        status += " isWritable";
    else status += " is not Writable";
    if (COM1port->isReadable())
        status += " isReadable";
    else status += " is not Readable";

    qDebug() << COM1port.get();
    return status;
}

void Core::makeCameraCapture()
{
    camPtr = new CamStreamTask;
    cam_pool.start(camPtr);
     connect(camPtr,&CamStreamTask::frameAvailable,
                this,  &Core::updateVideoFrame);
}

void Core::stopCameraCapture()
{
    camPtr->onStop();
}

bool Core::printScreen()
{
  const QString Data_dir = QString("DATA/DATA ")
            + QDate::currentDate().toString(QString("dd_MM_yyyy")) + QString(" ")
                + QTime::currentTime().toString(QString("hh_mm_ss"));

  QString path = QFileDialog::getSaveFileName
                    (nullptr,"SAVE IMAGE",Data_dir,"*.png");
  return lastFrame->save(path,"PNG");
}

void Core::updateVideoFrame(const QImage& frame_img)
{
   lastFrame = std::make_unique<QImage> (frame_img);
   framePointer = std::make_unique<Core::PXitem>
           (QPixmap::fromImage(frame_img));
   scene.addItem(framePointer.get());
}


void Core::infiniteAutoFocusProcess()
{
    if (autoFocusSemaphore == false) {
        runFocus->onStop();
        return;
    }
    runFocus = new AutoFocusRunnable;
    autoFocus_threadPool.start(runFocus);
    connect(runFocus,&AutoFocusRunnable::newValueFocus,
            this,&Core::updateFocusQualityBar);

}

void Core::setAutoFocusSemaphore(bool startOrStop)
{
    if (!camPtr)
    {
        return;
    }
    autoFocusSemaphore = startOrStop;
    infiniteAutoFocusProcess();
}


bool Core::appIsReadyForTraverseWalk()
{
    return camPtr and COM1port and COM1port->isConnected() and COM1port->isWritable() and COM1port->isReadable();
}

void Core::traverseWalkOnImage() {
    if (appIsReadyForTraverseWalk() == false) {
        return;
    }


    CellsSelecter::sharedSelecter().select(*lastFrame);
}


void VScene::wheelEvent(QGraphicsSceneWheelEvent * e)
{

}









