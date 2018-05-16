#include "autofocusmath.h"



qreal AutoFocusMath::pushNextFrameImage(const QImage &frame)
{
    qreal newValue = valueOfDefocusingCurve(frame);
    currentMaxFocusValue = std::max(currentMaxFocusValue,newValue);
    return  newValue;
}

qreal AutoFocusMath::getCurrentMaxFocusValue() const
{
    return currentMaxFocusValue;
}

void AutoFocusMath::calculateFocusValueWithFrame(const QImage &frame,
                                                 void (*invoke)(qreal focusValue))
{
    invoke(valueOfDefocusingCurve(frame));
}

qreal AutoFocusMath::defocusingCurveLaplace(const QImage& currentFrameImage)
{
    qreal Lk = 0;
    for (qint32 i = 1; i < currentFrameImage.height() - 1; ++i)
        for (qint32 j = 1; j < currentFrameImage.width() - 1; ++j)
        {
           qreal laplacian = (1/6) * (4 * brignessInPos(i,j,currentFrameImage) -
                 (brignessInPos(i - 1,j,currentFrameImage) + brignessInPos(i,j - 1,currentFrameImage)
                + brignessInPos(i + 1,j,currentFrameImage) + brignessInPos(i, j + 1,currentFrameImage)));
           Lk += std::abs(laplacian);
        }
    return Lk;
}

qreal AutoFocusMath::defocusingCurveStandardDeviation(const QImage& currentFrameImage)
{
    qreal Gk = 0;
    qint64 averageBrightness = 0;

    for (qint32 i = 0; i < currentFrameImage.height(); ++i)
        for (qint32 j = 0; j < currentFrameImage.width(); ++j)
        {
            averageBrightness += brignessInPos(i,j,currentFrameImage);
        }

    qint64 sizeOfImage = currentFrameImage.height() * currentFrameImage.width();
    if (sizeOfImage == 0) sizeOfImage = 1;

    averageBrightness /= sizeOfImage;

    for (qint32 i = 0; i < currentFrameImage.height(); ++i)
        for (qint32 j = 0; j < currentFrameImage.width(); ++j)
        {
            Gk += std::pow(brignessInPos(i,j,currentFrameImage) - averageBrightness, 2);
        }
    return Gk;
}

qreal AutoFocusMath::valueOfDefocusingCurve(const QImage& currentFrameImage)
{
    return 0.5 * (defocusingCurveStandardDeviation(currentFrameImage)
                  + defocusingCurveLaplace(currentFrameImage));
}

qint32 AutoFocusMath::brignessInPos(qint32 xPos, qint32 yPos, const QImage& currentFrameImage)
{
    QRgb pixel = currentFrameImage.pixel(xPos,yPos);
    return (qRed(pixel) + qBlue(pixel) + qGreen(pixel)) / 3;
}
