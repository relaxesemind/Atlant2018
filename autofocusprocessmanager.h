#ifndef AUTOFOCUSPROCESSMANAGER_H
#define AUTOFOCUSPROCESSMANAGER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QRunnable>
#include <QThreadPool>
#include <QImage>
#include <cmath>

#include "core.h"
#include "portcontroller.h"

class Core;

class AutoFocusMath : public QObject
{
    Q_OBJECT
private:
    AutoFocusMath() = default;
    AutoFocusMath(const AutoFocusMath&);
    AutoFocusMath& operator=( AutoFocusMath&);

public:
    static AutoFocusMath& getInstance() {
        static AutoFocusMath sharedInstance;
        return sharedInstance;
    }
    qreal currentMaxFocusValue;
public slots:

    qreal getCurrentMaxFocusValue() const;
    qreal valueOfDefocusingCurve(const QImage& currentFrameImage);


signals:
    void readyToGetNewFrame();

private:
    qreal defocusingCurveLaplace(const QImage& currentFrameImage);
    qreal defocusingCurveStandardDeviation(const QImage& currentFrameImage);
    qint32 brignessInPos(qint32 xPos, qint32 yPos, const QImage& currentFrameImage);
};


class AutoFocusProcessManager : public QObject
{
    Q_OBJECT
private:
    AutoFocusProcessManager() = default;
    AutoFocusProcessManager(const AutoFocusProcessManager&);
    AutoFocusProcessManager& operator=( AutoFocusProcessManager&);

public:
    static AutoFocusProcessManager& sharedManager() {
        static AutoFocusProcessManager shared;
        return shared;
    }

    qreal lastCalculatedValue;
    int currentSteps;

    void setCore(Core *core);
    void getCurrentFrame();

signals:
    void incZ(int steps);
    void decZ(int steps);
    void focusingValueWasChanged(int value);

public slots:
    int start();
    void stop();
protected:
    Core *corethis;
private:
   QImage currentFrame;
};

class AutoFocusRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit AutoFocusRunnable( QObject* parent = 0 );

public slots:
    void onStop();
    void onStart();

signals:
    void newValueFocus(int value);

protected:
    void run();

private:
    bool m_stopped;
};

#endif // AUTOFOCUSPROCESSMANAGER_H
