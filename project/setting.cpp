#include "setting.h"
#include "ui_setting.h"
#include "newgame.h"

setting::setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);
}

setting::~setting()
{
    delete ui;
}

void setting::on_btn_Start_clicked()
{
    this->close();
    _data = new database(boardSize, firstRock, obstacleCount);
    _newgame = new class newgame(boardSize, firstRock, isTimer, time, addTime, 0, nullptr, _data);
    _newgame->show();

    _newgame->stoneButtons.resize(boardSize);
    for (int i = 0; i < boardSize; ++i) {
        _newgame->stoneButtons[i].resize(boardSize);
        for (int j = 0; j < boardSize; ++j) {
            _newgame->stoneButtons[i][j] = nullptr;
        }
    }

    if(isTimer == true){
        if(_data->getPlayer() == 0)
            _newgame->blackTimer->start();
        else
            _newgame->whiteTimer->start();
    }

}


void setting::on_pushButton_2_clicked()
{
    this->close();
}


void setting::on_comboBox_currentIndexChanged(int index)
{
    if(index == 0)
        boardSize = 6;
    if(index == 1)
        boardSize = 8;
    if(index == 2)
        boardSize = 10;
    if(index == 3){
        boardSize = 12;
    }
}



void setting::on_firstPlayer_currentIndexChanged(int index)
{
    if(index == 0)
        firstRock = 0;
    if(index == 1)
        firstRock = 1;
    if(index == 2) //랜덤
        firstRock = QRandomGenerator::global()->bounded(2);

}



void setting::on_obstacleCount_currentIndexChanged(int index)
{
    if(index == 0)
    {
        obstacleCount = 0;
    }
    else
    {
        obstacleCount = index * 2;
    }

}




void setting::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        isTimer = true;
    } else {
        isTimer = false;
    }
}



void setting::on_spinBox_valueChanged(int arg1)
{
    time = arg1;
}


void setting::on_spinBox_2_valueChanged(int arg1)
{
    addTime = arg1;
}

