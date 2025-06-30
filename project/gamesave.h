#ifndef GAMESAVE_H
#define GAMESAVE_H

#include <QVector>
#include <QString>
#include <QList>
#include <QDataStream>
#include <QtCore/qpoint.h>


struct GameState {

    //보드 상태
    QVector<QVector<int>> boardState;

    //턴 횟수
    int turnCount;

    //현재 플레이어
    int currentPlayer;

    //흑돌 점수
    int scoreBlack;

    //백돌 점수
    int scoreWhite;

    bool isTimer;

    //총 시간
    int time;

    //증초시간
    int addTime;


    //제한 시간(현재)
    int blackTime; // 검은 돌의 남은 시간
    int whiteTime; // 흰 돌의 남은 시간



    //마지막 위치
    QPoint lastMovePosition;

    friend QDataStream &operator<<(QDataStream &out, const GameState &state) {
        out << state.boardState
            << state.turnCount
            << state.currentPlayer
            << state.scoreBlack
            << state.scoreWhite
            << state.blackTime
            << state.whiteTime
            << state.time
            << state.addTime
            << state.isTimer
            << state.lastMovePosition;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, GameState &state) {
        in >> state.boardState
            >> state.turnCount
            >> state.currentPlayer
            >> state.scoreBlack
            >> state.scoreWhite
            >> state.blackTime
            >> state.whiteTime
            >> state.time
            >> state.addTime
            >> state.isTimer
            >> state.lastMovePosition;
        return in;
    }


};

struct GameSave {

    //보드 사이즈
    int boardSize;

    //타이머 여부
    bool isTimer;

    //총 시간
    int time;

    //증초시간
    int addTime;

    //턴수
    int turnCount;

    //돌
    int firstRock;

    //장애물 수
    int obstacleCount;

    //히스토리
    QList<GameState> history;

    friend QDataStream &operator<<(QDataStream &out, const GameSave &save) {
        out << save.boardSize
            << save.isTimer
            << save.time
            << save.addTime
            << save.turnCount
            << save.firstRock
            << save.obstacleCount
            << save.history;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, GameSave &save) {
        in >> save.boardSize
            >>save.isTimer
            >> save.time
            >> save.addTime
            >> save.turnCount
            >> save.firstRock
            >> save.obstacleCount
            >> save.history;
        return in;
    }

    int getBoardSize() const;
    int getBoardRock(int x, int y) const;

};


//데이터 가져오기
bool saveGameToFile(const QString &filename, const GameSave &saveData);

//데이터 저장
bool loadGameFromFile(const QString &filename, GameSave &saveData);



#endif // GAME_SAVE_H
