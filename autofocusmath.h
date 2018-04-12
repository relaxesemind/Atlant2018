#ifndef AUTOFOCUSMATH_H
#define AUTOFOCUSMATH_H

#include <QImage>
#include <cmath>


class AutoFocusMath : public QObject
{
    Q_OBJECT
private:
    AutoFocusMath();
    AutoFocusMath(const AutoFocusMath&);
    AutoFocusMath& operator=( AutoFocusMath&);

public:
    static AutoFocusMath& getInstance() {
        static AutoFocusMath sharedInstance;
        return sharedInstance;
    }
public slots:

    void pushNextFrameImage(const QImage& frame);
    qreal getCurrentMaxFocusValue() const;

signals:
    void readyToGetNewFrame();

private:
    qreal currentMaxFocusValue;
    qreal defocusingCurveLaplace(const QImage& currentFrameImage);
    qreal defocusingCurveStandardDeviation(const QImage& currentFrameImage);
    qreal valueOfDefocusingCurve(const QImage& currentFrameImage);
    qint32 brignessInPos(qint32 xPos, qint32 yPos, const QImage& currentFrameImage);
};

#endif // AUTOFOCUSMATH_H
