#ifndef REPLAY_H
#define REPLAY_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QtWidgets/qslider.h>
#include "gamesave.h"

class replay : public QWidget {
    Q_OBJECT

public:
    explicit replay(QWidget *parent = nullptr);
    void loadReplay(const QString &path);

private slots:
    void showPrevTurn();
    void showNextTurn();

private:
    void updateBoard();

    GameSave saveData;
    int currentTurn;

    QGridLayout *gridLayout;
    QVector<QVector<QPushButton*>> board;

    QLabel *turnLabel;
    QLabel *blackScore;
    QLabel *whiteScore;
    QSlider *turnSlider;
    QPushButton *MoveMainbtn;

    QVector<QPushButton*> dolOverlays;

};

#endif // REPLAYWINDOW_H
