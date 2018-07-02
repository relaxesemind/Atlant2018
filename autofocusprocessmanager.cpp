#include "autofocusprocessmanager.h"


void AutoFocusProcessManager::setCore(Core *core)
{
    corethis = core;
}

void AutoFocusProcessManager::getCurrentFrame()
{
    currentFrame = *corethis->lastFrame.get();
}

int AutoFocusProcessManager::start()
{
    lastCalculatedValue = AutoFocusMath::getInstance().valueOfDefocusingCurve(currentFrame);
  //  emit focusingValueWasChanged(static_cast<int> (lastCalculatedValue));
    return static_cast<int> (lastCalculatedValue);
}

void AutoFocusProcessManager::stop()
{

}

qreal AutoFocusMath::getCurrentMaxFocusValue() const
{
    return currentMaxFocusValue;
}

qreal AutoFocusMath::defocusingCurveLaplace(const QImage& currentFrameImage)
{
    qreal Lk = 0;
    for (qint32 y = 1; y < currentFrameImage.height() - 1; ++y)
        for (qint32 x = 1; x < currentFrameImage.width() - 1; ++x)
        {
           qreal laplacian = (1/6) * (4 * brignessInPos(x,y,currentFrameImage) -
                 (brignessInPos(x - 1,y,currentFrameImage) + brignessInPos(x,y - 1,currentFrameImage)
                + brignessInPos(x + 1,y,currentFrameImage) + brignessInPos(x, y + 1,currentFrameImage)));
           Lk += std::abs(laplacian);
        }
    return Lk;
}

qreal AutoFocusMath::defocusingCurveStandardDeviation(const QImage& currentFrameImage)
{
    qreal Gk = 0;
    qint64 averageBrightness = 0;

 //   qDebug() << currentFrameImage;
//
//    for(qint32 y = 0; y < img.height(); ++y)
//      for(qint32 x = 0; x < img.width(); ++x) {

//          if ((!array[y][x]) and (isBlack(x,y,img))) {
//              fill(img,array,x,y,L++);
//          }
//      }

    for (qint32 y = 0; y < currentFrameImage.height(); ++y)
        for (qint32 x = 0; x < currentFrameImage.width(); ++x)
        {
            averageBrightness += brignessInPos(x,y,currentFrameImage);
        }

    qint64 sizeOfImage = currentFrameImage.height() * currentFrameImage.width();
    if (sizeOfImage == 0) sizeOfImage = 1;

    averageBrightness /= sizeOfImage;

    for (qint32 y = 0; y < currentFrameImage.height(); ++y)
        for (qint32 x = 0; x < currentFrameImage.width(); ++x)
        {
            Gk += std::pow(brignessInPos(x,y,currentFrameImage) - averageBrightness, 2);
        }
    return Gk;
}

qreal AutoFocusMath::valueOfDefocusingCurve(const QImage& currentFrameImage)
{
    qreal newValue = 0.5 * (defocusingCurveStandardDeviation(currentFrameImage)
                            + defocusingCurveLaplace(currentFrameImage));

    newValue /= 1000000;
     //qDebug () << "newValue = " << newValue;
    currentMaxFocusValue = std::max(currentMaxFocusValue,newValue);
 //   qDebug () << currentMaxFocusValue;
    emit readyToGetNewFrame();
    return  newValue;
}

qint32 AutoFocusMath::brignessInPos(qint32 xPos, qint32 yPos, const QImage& currentFrameImage)
{
    QRgb pixel = currentFrameImage.pixel(xPos,yPos);
    return (qRed(pixel) + qBlue(pixel) + qGreen(pixel)) / 3;
}

AutoFocusRunnable::AutoFocusRunnable( QObject* parent)
{
    m_stopped = false;
}

void AutoFocusRunnable::onStop()
{
    m_stopped = true;
}

void AutoFocusRunnable::onStart()
{
    m_stopped = false;
}

bool AutoFocusRunnable::isActive()
{
    return !m_stopped;
}


/**
 * Общий алгоритм движения камеры при автофокусировке:
 * + 1) посчитать значение в текущем положении (усредненное 20 кадров)
 * 2) Двигемся вверх и считаем усредненное значение "налету"
 * 2.1) Если значение увеличивается то продолжаем движение
 * 2.1.1) Если оно начало уменьшаться то останавливаемся - ТОЧКА НАЙДЕНА
 * 2.2) Если значение уменьшается то возвращаемся в исходное положение
 * 2.2.1) Двигаемся вниз и считаем усредненное значение "налету"
 * 2.2.2) Если оно начало уменьшаться то мы значально в максимуме - ТОЧКА НАЙДЕНА
 * 2.2.3) Если оно начало увеличиваться то продолжаемся движение до уменьшения - ТОЧКА
 *
 **/

void AutoFocusRunnable::run()
{
    forever {
        if( m_stopped ) {
            break;
        }

        AutoFocusProcessManager::sharedManager().getCurrentFrame();
        int value = AutoFocusProcessManager::sharedManager().start();
        emit newValueFocus(value);
    }
}
