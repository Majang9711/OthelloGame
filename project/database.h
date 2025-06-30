#ifndef DATABASE_H
#define DATABASE_H
#include <QVector>

class database
{
private:
    int boardsize;
    int player;
    int obstacleCount;
    int turnCount;
    int scoreWhite, scoreBlack;
    bool isContinue;

public:
    QVector<QVector<int>> boardStateData;
    database(int size, int p, int oC)
    {

        boardsize = size;
        boardStateData.resize(boardsize);

        player = p;
        obstacleCount = oC;
        turnCount = 0;
        scoreWhite = 0;
        scoreBlack = 0;
        for(int i = 0; i < boardsize; i++)
        {
            boardStateData[i].resize(boardsize);
            for(int j = 0; j < boardsize; j++)
            {
                //돌이 없는 상태
                boardStateData[i][j] = -1;
            }

        }

    }
    //보드상태 겟터
    QVector<QVector<int>> getBoardState() const
    {
        return boardStateData;
    }


    //보드사이즈 겟터
    int getBoardSize()
    {
        return boardsize;
    }

    //보드사이즈 셋터
    void setBoardSize(int size)
    {
        boardsize = size;
    }

    //이어하기 셋터
    void setIsContinue(bool a)
    {
        isContinue = a;
    }

    //이어하기 겟터
    bool getIsContinue()
    {
        return isContinue;
    }

    //턴 겟터
    int getTurnCount()
    {
        return turnCount;
    }


    //백돌 점수 add
    void addScoreWhite(int cnt)
    {
         scoreWhite += cnt;
    }

    //백돌 점수 set
    void setScoreWhite(int cnt)
    {
        scoreWhite = cnt;
    }

    //백돌 점수 겟터
    int getScoreWhite()
    {
        return scoreWhite;
    }

    //흑덜 점수 add
    void addScoreBlack(int cnt)
    {
        scoreBlack += cnt;
    }

    //흑덜 점수 set
    void setScoreBlack(int cnt)
    {
        scoreBlack = cnt;
    }

    //흑돌 점수 겟터
    int getScoreBlack()
    {
        return scoreBlack;
    }

    //턴 셋터
    void setTurnCount(int cnt)
    {
        turnCount = cnt;
    }

    //턴 add
    void addTurnCount(int cnt)
    {
        turnCount += cnt;
    }

    //플레이어 겟터 (1=백, 0=흑)
    int getPlayer()
    {
        return player;
    }

    //플레이어 설정
    void setPlayer(int p)
    {
        player = p;
    }

    //장애물 겟터
    int getObstacleCount()
    {
        return obstacleCount;
    }

    //돌 위치 저장
    void setBoardRock(int x, int y, int rock)
    {
        boardStateData[x][y] = rock;
    }

    //돌 무슨 색인지 반환 (1=백, 0=흑, 없음=-1, 장애물= -2)
    int getBoardRock(int x, int y)
    {
        return boardStateData[x][y];
    }

};

#endif // DATABASE_H
