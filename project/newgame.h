#ifndef NEWGAME_H
#define NEWGAME_H

#include <QWidget>
#include <QtWidgets/qpushbutton.h>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include "database.h"
#include "gamesave.h"
#include "timer.h"

namespace Ui {
class newgame;
}

class newgame : public QWidget
{
    Q_OBJECT

public:
    //새게임용도
    explicit newgame(int size, int firstRock, bool isTimer, int time, int addTime, int case_, QWidget *parent = nullptr, database *data = nullptr, const GameSave* saveData = nullptr);

    ~newgame();

    onTimer* whiteTimer;
    onTimer* blackTimer;
    QVector<QVector<QPushButton*>> stoneButtons;  // 돌을 추적할 2차원 벡터
    database *data;

    bool getIsTimer()
    {
        return isTimer;
    }


private:
    QGridLayout *gridLayout;
    QLabel *sideText1;
    QLabel *playerLabel;
    QLabel *whiteCnt;
    QLabel *blackCnt;
    QLabel *timeLabel;
    QPushButton *MoveMainbtn;;
    QPushButton *now_btn;
    QPushButton *Undobtn;
    QPushButton *Savebtn;

    QVector<QVector<QPushButton*>> board;


    GameSave saveData;


    int lastX = -1;
    int lastY = -1;
    int size;
    int whiteTime;
    int blackTime;
    int incrementTime;
    bool useFischerMode;
    int turnCount;
    int currentPlayer;


    //타이머 동작 여부
    bool isTimer;

    //총 시간
    int time;

    //증초시간
    int addTime;


    //돌 착수
    void setStoneCarving(int x, int y, int color, int size);

    //장애물 랜덤 설정
    void setObstacle(int size, int obstacleCount);

    //초기 착수 상태
    void baseSetting(int size);

    //라벨들 업데이트
    void updateInfo();

    //뒤집기
    void flipStone(int x, int y, int color);

    //착수 가능하면 고
    bool isCaving(int x, int y, int color);

    //착수 가능여부
    bool onPass(int color);

    //착수 가능한 부분 색상 번경
    void updateIsCaving(int color);

    //게임상태 리플레이 저장
    void onGameSave(int blackTime, int whiteTime, int case_, QString name);

    //게임 중간에 저장(파일로)
    void onGameingSave(int blackTime, int whiteTime, int case_, QString name);

    //턴별로 히스토리 저장
    void saveTurnStateToHistory(int blackTime, int whiteTime);

    //턴 변경할떄 시간 스탑 스타트
    void switchTurn();

    //턴 되돌리기
    void undoMove(int boardsize);

    //UI보드 업데이투
    void updateCellUI(int i, int j, int stone);

    //불러오기 클릭시 로드
    void onLoad();

    //UI재구성
    void refreshUIFromBoard();


};

#endif // NEWGAME_H
