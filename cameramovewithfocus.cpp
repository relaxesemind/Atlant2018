#include "cameramovewithfocus.h"

CameraMoveWithFocus::CameraMoveWithFocus(QObject *parent) : QObject(parent)
{
    m_stopped = false;
    runFocus = new AutoFocusRunnable;
    autoFocus_threadPool.start(runFocus);
    connect(runFocus,&AutoFocusRunnable::newValueFocus,this,
            &CameraMoveWithFocus::getValueFromAutoFocus);
}

CameraMoveWithFocus::~CameraMoveWithFocus()
{
    if (runFocus) {
        runFocus->onStop();
    }
    autoFocus_threadPool.waitForDone();
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
    currentValueFocus = value;
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
        if( m_stopped ) {
            break;
        }

    }
}
