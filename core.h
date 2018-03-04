#ifndef CORE_H

#define CORE_H

#include "controller.h"
#include "cameracontroller.h"

#include <QObject>
#include <memory>
#include <QThread>
#include <QDebug>
#include <QString>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QImage>

class Core : public QObject
{
    Q_OBJECT
    using PXitem = QGraphicsPixmapItem;
    using pItem = std::unique_ptr<PXitem>;
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

     QGraphicsScene scene;
signals:

public slots:
    void makeConnetionWithPort();
    void disconnectPort();
    QString moveToRight(int steps = 2);
    QString checkPortStatus();
    void makeCameraCapture();
    void stopCameraCapture();
    void updateVideoFrame(const QImage&);

private:
    pItem framePointer;
    QThreadPool m_pool;
};

#endif // CORE_H
