#include "cameramovewithfocus.h"

CameraMoveWithFocus::CameraMoveWithFocus(Core *core, QObject *parent) : QObject(parent)
{
    corethis = core;
    m_stopped = false;
    maxYposition = 0;
    direction = true;
    currentPosition = maxYposition;
    lastFocusValue = 0;
    repetition = 0;
    confidenceInterval = 120;
    stepsNumberIteration = 75;
    maxValueFocus = 0;
}

CameraMoveWithFocus::~CameraMoveWithFocus()
{

}

void CameraMoveWithFocus::onStop()
{
    m_stopped = true;
}

void CameraMoveWithFocus::onStart()
{
    m_stopped = false;
}

void CameraMoveWithFocus::getValueFromAutoFocus(int value)
{
    newestFocusValue = value;
}

void CameraMoveWithFocus::needToMoveUp(int steps)
{
    corethis->moveToUp(steps);
}

void CameraMoveWithFocus::needToMoveDown(int steps)
{
    corethis->moveToDown(steps);
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

void CameraMoveWithFocus::run()
{
    forever {
        if( m_stopped || (currentPosition - stepsNumberIteration) > maxYposition ) {
            break;
        }
    qDebug () << "0000";
    this->thread()->sleep(1);
    qDebug () << "11111";
     if (!makeDecision()) {
         qDebug () << "22222";
         moveCamera();
     } else {
         qDebug () << "3333";
         m_stopped = true;
     }
   }
}

bool CameraMoveWithFocus::makeDecision()
{
    int diff = newestFocusValue - maxValueFocus;
    maxValueFocus = std::max(maxValueFocus, newestFocusValue);
    qDebug() << "newestFocusValue " << newestFocusValue << " maxValueFocus " << maxValueFocus;
    qDebug() << "diff = " << diff;
    if (diff < confidenceInterval) {
       ++repetition;
    } else {
        repetition = 0;
    }
    return repetition > 4;
}

void CameraMoveWithFocus::moveCamera()
{
    if (direction) {
        qDebug () <<"Down";
        needToMoveDown(75);
        currentPosition -= 75;
     } else {
        qDebug () <<"UP";
        needToMoveUp(75);
        currentPosition += 75;
    }
}





















