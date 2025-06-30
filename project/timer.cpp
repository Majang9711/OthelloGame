#include "timer.h"

onTimer::onTimer(int baseSeconds, int incrementSeconds, QObject *parent)
    : QObject(parent),
    m_baseTime(baseSeconds),
    m_increment(incrementSeconds),
    m_remainingTime(baseSeconds)
{
    connect(&m_timer, &QTimer::timeout, this, &onTimer::onTimeout);
    m_timer.setInterval(1000);
}

void onTimer::start()
{
    m_timer.start();
}

void onTimer::stop()
{
    m_timer.stop();
}

void onTimer::reset()
{
    m_remainingTime = m_baseTime;
    emit timeChanged(m_remainingTime);
}

void onTimer::moveMade()
{
    m_remainingTime += m_increment;
    emit timeChanged(m_remainingTime);
}

void onTimer::tick()
{
    if (m_remainingTime > 0) {
        m_remainingTime--;
        emit timeChanged(m_remainingTime);
        if (m_remainingTime == 0) {
            m_timer.stop();
            emit timeExpired();
        }
    }
}

int onTimer::getRemainTime()
{
    return m_remainingTime;
}

void onTimer::setRemainTime(int time)
{
    m_remainingTime = time;
}

void onTimer::onTimeout()
{
    tick();
}
