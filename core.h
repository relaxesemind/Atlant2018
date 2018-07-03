#ifndef CORE_H

#define CORE_H

#include "portcontroller.h"
#include "cameracontroller.h"
#include "autofocusprocessmanager.h"
#include "cameramovewithfocus.h"

#include <QObject>
#include <memory>
#include <QThread>
#include <QDebug>
#include <QString>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneWheelEvent>
#include <QPixmap>
#include <QImage>
#include <functional>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

bool extern portisWorking;

class AutoFocusRunnable;
class CameraMoveWithFocus;

class VScene : public QGraphicsScene
{
    Q_OBJECT
signals:
    void zoomIn();
    void zoomOut();
protected:
    void wheelEvent(QGraphicsSceneWheelEvent*) override;
};

class Core : public QObject
{
    Q_OBJECT
    using PXitem = QGraphicsPixmapItem;
    using pItem = std::unique_ptr<PXitem>;
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

     VScene scene;
     std::unique_ptr<QImage> lastFrame;
signals:

    void feedBackFromPort(const QString& message);
    void updateFocusQualityBar(int value);
    void newFrameIsReady(const QImage& frame);


public slots:

    void traverseWalkOnImage();

    void makeConnetionWithPort();
    void disconnectPort();
    QString moveToRight(int steps);
    QString moveToLeft(int steps);
    QString moveToForward(int steps);
    QString moveToBackward(int steps);
    QString moveToUp(int steps);
    QString moveToDown(int steps);
    QString checkPortStatus();
    void makeCameraCapture();
    void stopCameraCapture();
    bool printScreen();
    void updateVideoFrame(const QImage&);
    void infiniteAutoFocusProcess();
    void setAutoFocusSemaphore(bool startOrStop);

    void autoFocusEnds(int z);

private:

    pItem framePointer;
    bool autoFocusSemaphore;
    QTimer m_timer;

    QThreadPool threadPool;

    CamStreamTask* camPtr;
    CameraMoveWithFocus *cameraMover;
    AutoFocusRunnable *autoFocusProcess;

    std::unique_ptr<PortController> COM1port;


    bool appIsReadyForTraverseWalk();
};


#endif // CORE_H
