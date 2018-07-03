#ifndef CAMERAMOVEWITHFOCUS_H
#define CAMERAMOVEWITHFOCUS_H

#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <map>

#include "portcontroller.h"
#include "autofocusprocessmanager.h"

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
class AutoFocusRunnable;
class CameraMoveWithFocus : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit CameraMoveWithFocus(QObject *parent = nullptr);
    ~CameraMoveWithFocus();

public slots:
    void onStop();
    void onStart();
    void getValueFromAutoFocus(int value);

signals:
    void complitionSignalWithZcoordinate( int z );
    void needToMoveUp(int steps);
    void needToMoveDown(int steps);
///Сообщаем Core о том что надо двинутся вверх или вниз. в run обрабатываем это
protected:
    void run();

private:
    bool m_stopped;
    int newestFocusValue;
    int lastFocusValue;
    int maxValueFocus;
    bool direction; // true down, false up
    int maxYposition;
    int currentPosition;
    int repetition;

    int stepsNumberIteration;
    int confidenceInterval;

    std::map<int, int> values;

    bool makeDecision();
    void moveCamera();
};

#endif // CAMERAMOVEWITHFOCUS_H
