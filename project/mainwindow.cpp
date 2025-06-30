#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "replay.h"
#include <QMessageBox>
#include "newgame.h"
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_newgame_clicked()
{
    setting = new class setting();
    setting->show();
}


void MainWindow::on_quit_clicked()
{
    QString filePath = QDir::homePath() + "/replay_save.dat";
    if (QFile::exists(filePath)) {
        if (QFile::remove(filePath)) {
            qDebug() << "파일 삭제 성공:" << filePath;
        } else {
            qDebug() << "파일 삭제 실패:" << filePath;
        }
    } else {
        qDebug() << "삭제할 파일이 존재하지 않음:" << filePath;

    }
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton quit;
    quit = QMessageBox::question(this, "게임 종료",
                                  "정말로 종료하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (quit == QMessageBox::Yes) {
        event->accept();
        on_quit_clicked();
    } else {
        event->ignore();
    }
}

void MainWindow::on_btn_newgame_2_clicked()
{
    setting->onContinueGame();

}


void MainWindow::on_btn_newgame_3_clicked()
{
    QString filePath = QDir::homePath() + "/replay_save.dat";
    if (QFile::exists(filePath)) {
        replay *replay = new class replay();
        replay->loadReplay(QDir::homePath() + "/replay_save.dat");
        replay->show();
    }
}


void MainWindow::on_btnComeSave_clicked()
{
    QString filePath = QDir::homePath() + "/save.dat";

    // 저장 파일 존재 확인
    if (!QFile::exists(filePath)) {
        QMessageBox::warning(this, "불러오기 실패", "저장된 게임이 없습니다.");
        return;
    }

    // 저장 데이터 구조체 생성 및 파일 로드
    GameSave saveData;
    if (!loadGameFromFile(filePath, saveData)) {
        QMessageBox::critical(this, "불러오기 오류", "게임 데이터를 불러오는 데 실패했습니다.");
        return;
    }

    //불러오기 성공
    qDebug("불러오기 성공");

    // 마지막 저장된 게임 상태 사용

    database *_data = new database(saveData.boardSize, saveData.firstRock, saveData.obstacleCount);


    // newgame 객체 생성 및 저장 데이터 전달
    // newgame *_newgame = new newgame(saveData, this);

    newgame *_nnewgame = new newgame(saveData.boardSize, saveData.firstRock, saveData.isTimer, saveData.time, saveData.addTime, 1, nullptr, _data, &saveData);
    _nnewgame->show();


    _nnewgame->stoneButtons.resize(saveData.boardSize);
    for (int i = 0; i < saveData.boardSize; ++i) {
        _nnewgame->stoneButtons[i].resize(saveData.boardSize);
        for (int j = 0; j < saveData.boardSize; ++j) {
            _nnewgame->stoneButtons[i][j] = nullptr;
        }
    }
}

