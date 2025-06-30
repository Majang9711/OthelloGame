#ifndef SETTING_H
#define SETTING_H

#include "newgame.h"
#include <QDialog>
#include <QRandomGenerator>

namespace Ui {
class setting;
}

class setting : public QDialog
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = nullptr);
    ~setting();

    int getBoardSize()
    {
        return boardSize;
    }

    int getFirstRock()
    {
        return firstRock;
    }

    int getObstacleCount()
    {
        return obstacleCount;
    }

    void onContinueGame()
    {
        if(_data->getIsContinue() == true)
        {
            _newgame->show();

            if(isTimer == true)
            {
                if(_newgame->data->getPlayer() == 0)
                {
                    _newgame->blackTimer->start();
                }else{
                     _newgame->whiteTimer->start();

                }
            }
        }

    }

private slots:
    void on_pushButton_2_clicked();

    void on_btn_Start_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_obstacleCount_currentIndexChanged(int index);

    void on_firstPlayer_currentIndexChanged(int index);

    void on_checkBox_stateChanged(int arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

private:
    Ui::setting *ui;

    //보드 사이즈
    int boardSize = 6;

    //먼저 공격 돌(기본 흑)
    int firstRock = 0;

    //장애물 개수
    int obstacleCount;

    //타이머 동작 여부
    bool isTimer = false;

    //총 시간
    int time;

    //증초시간
    int addTime;

    newgame *_newgame = nullptr;
    database *_data = nullptr;
};

#endif // SETTING_H
