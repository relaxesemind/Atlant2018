#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>

#include <memory>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>

class CamStreamTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit CamStreamTask( QObject* parent = 0 );

public slots:
    void onStop();
    void onStart();

signals:
    void frameAvailable( const QImage& img );

protected:
    void run();

private:
    bool m_stopped;
    cv::VideoCapture cap;
};


#endif // CAMERACONTROLLER_H
