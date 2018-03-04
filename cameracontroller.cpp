#include "cameracontroller.h"

CamStreamTask::CamStreamTask(QObject *parent)
{
    m_stopped = false;
}


void CamStreamTask::onStop()
{
    m_stopped = true;
}

void CamStreamTask::onStart()
{
    m_stopped = false;
}

void CamStreamTask::run()
{
    if(!cap.open(0)){
        return;
    }

    forever {
        if( m_stopped ) {
            break;
        }

        cv::Mat frame;
        cap >> frame;
        if( frame.empty() ) {
            m_stopped = true;
            continue;
        }

        cv::Mat frameRGB;

        cv::cvtColor( frame, frameRGB, CV_BGR2RGB );

        QImage img(
            reinterpret_cast<const uchar*> (frameRGB.data),
            frameRGB.cols,
            frameRGB.rows,
            frameRGB.step,
            QImage::Format_RGB888
        );
        emit frameAvailable(img.copy());
    }
}
