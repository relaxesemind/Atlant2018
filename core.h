#ifndef CORE_H

#define CORE_H

#include "portcontroller.h"
#include "cameracontroller.h"

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
#include <QDate>
#include <QTime>
#include <QFileDialog>

bool extern portisWorking;


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
signals:

    void feedBackFromPort(const QString& message);

public slots:
    void makeConnetionWithPort();
    void disconnectPort();
    QString moveToRight(int steps);
    QString moveToLeft(int steps);
    QString moveToUp(int steps);
    QString moveToDown(int steps);
    QString checkPortStatus();
    void makeCameraCapture();
    void stopCameraCapture();
    bool printScreen();
    void updateVideoFrame(const QImage&);

private:

    pItem framePointer;
    QThreadPool m_pool;
    std::unique_ptr<PortController> COM1port;
    CamStreamTask* camPtr;
    std::unique_ptr<QImage> lastFrame;
};


#endif // CORE_H
