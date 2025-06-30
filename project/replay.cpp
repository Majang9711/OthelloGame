#include "replay.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDataStream>
#include <QtCore/qtimer.h>

replay::replay(QWidget *parent) : QWidget(parent), currentTurn(0) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *boardLayout = new QHBoxLayout();

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);

    turnLabel = new QLabel("0 / 0", this);
    blackScore = new QLabel("흑돌: 0", this);
    whiteScore = new QLabel("백돌: 0", this);

    MoveMainbtn = new QPushButton("메인화면", this);
    MoveMainbtn->setFixedSize(150, 70);
    blackScore->setFixedWidth(150);
    whiteScore->setFixedWidth(150);

    QVBoxLayout *sideLayout = new QVBoxLayout();
    sideLayout->addWidget(blackScore);
    sideLayout->addWidget(whiteScore);
    sideLayout->addWidget(MoveMainbtn);
    sideLayout->addStretch();

    boardLayout->addLayout(gridLayout);
    boardLayout->addLayout(sideLayout);

    turnSlider = new QSlider(Qt::Horizontal, this);
    turnSlider->setMinimum(0);
    turnSlider->setValue(0);

    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(turnLabel);
    sliderLayout->addWidget(turnSlider);


    mainLayout->addLayout(boardLayout);
    mainLayout->addLayout(sliderLayout);

    connect(turnSlider, &QSlider::valueChanged, this, [=](int value){
        currentTurn = value;
        updateBoard();
    });

    connect(MoveMainbtn, &QPushButton::clicked, this, [=](){
        this->close();
    });

    mainLayout->addWidget(turnSlider);

}

void replay::loadReplay(const QString &path) {
    GameSave loaded;
    if (!loadGameFromFile(path, loaded)) {
        qDebug() << "리플레이 파일 로드 실패";
        return;
    }


    saveData = loaded;
    currentTurn = 0;
    gridLayout->setSpacing(0);

    int size = saveData.boardSize;
    board.resize(size);
    for (int i = 0; i < size; ++i) {
        board[i].resize(size);
        for (int j = 0; j < size; ++j) {
            QPushButton *btn = new QPushButton(this);
            btn->setFixedSize(40, 40);
            btn->setStyleSheet("background-color: #EEE; border: 1px solid black;");
            btn->setEnabled(false);
            gridLayout->addWidget(btn, i, j);
            board[i][j] = btn;
        }
    }


    QTimer::singleShot(0, this, [=]() {
        updateBoard();
    });

    turnSlider->setMaximum(saveData.history.size() - 1);
    turnSlider->setValue(0);

}

void replay::updateBoard() {
    if (currentTurn < 0 || currentTurn >= saveData.history.size()-1) return;

    const GameState &state = saveData.history[currentTurn];
    int size = state.boardState.size();

    // 기존 돌 제거
    for (auto btn : dolOverlays) {
        btn->deleteLater();
    }
    dolOverlays.clear();

    QPoint last = state.lastMovePosition;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int val = state.boardState[i][j];

            board[i][j]->setStyleSheet("background-color: #EEE; border: 1px solid gray;");

            if (val == -2) {
                board[i][j]->setStyleSheet("background-color: blue; border: 1px solid gray;");
            }

            // 일반 돌 먼저 생성 (마지막 착수 위치 제외)
            if ((val == 0 || val == 1) && last != QPoint(j, i)) {
                QPushButton *dol = new QPushButton(board[i][j]); // 부모를 셀로 설정
                dol->setFixedSize(36, 36);
                QString color = (val == 0) ? "black" : "white";
                dol->setStyleSheet(QString("background-color: %1; border-radius: 18px; border: 1px solid gray;").arg(color));
                dol->move(2, 2); // 셀 내부 위치 보정 (40x40 셀 기준 중앙)
                dol->setEnabled(false);
                dol->show();
                dolOverlays.push_back(dol);
            }
        }
    }

    // 마지막 착수 위치 돌 따로 생성 (항상 마지막에)
    if (last.x() >= 0 && last.y() >= 0) {
        int val = state.boardState[last.y()][last.x()];

        board[last.y()][last.x()]->setStyleSheet("background-color: #EEE; border: 2px solid red;");

        if (val == 0 || val == 1) {
            QPushButton *dol = new QPushButton(board[last.y()][last.x()]);
            dol->setFixedSize(36, 36);
            QString color = (val == 0) ? "black" : "white";
            dol->setStyleSheet(QString("background-color: %1; border-radius: 18px; border: 1px solid gray;").arg(color));
            dol->move(2, 2);
            dol->setEnabled(false);
            dol->show();
            dolOverlays.push_back(dol);
        }
    }

    turnLabel->setText(QString("%1 / %2").arg(currentTurn + 1).arg(saveData.history.size()-1));
    blackScore->setText("흑돌: " + QString::number(state.scoreBlack));
    whiteScore->setText("백돌: " + QString::number(state.scoreWhite));

    turnSlider->blockSignals(true);
    turnSlider->setValue(currentTurn);
    turnSlider->blockSignals(false);

}



void replay::showPrevTurn() {
    if (currentTurn > 0) {
        --currentTurn;
        updateBoard();
    }
}

void replay::showNextTurn() {
    if (currentTurn < saveData.history.size() - 1) {
        ++currentTurn;
        updateBoard();
    }
}
