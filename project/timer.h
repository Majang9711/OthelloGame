#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>

class onTimer : public QObject
{
    Q_OBJECT

public:
    explicit onTimer(int baseSeconds, int incrementSeconds, QObject *parent = nullptr);

    void start();
    void stop();
    void reset();
    void moveMade();
    int getRemainTime();
    void setRemainTime(int time);

signals:
    void timeChanged(int secondsLeft);
    void timeExpired();

private slots:
    void onTimeout();

private:
    void tick();

    QTimer m_timer;
    int m_baseTime;
    int m_increment;
    int m_remainingTime;
};

#endif // TIMER_H
