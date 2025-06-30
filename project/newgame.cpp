#include "newgame.h"
#include "database.h"
#include "gamesave.h"
#include "timer.h"
#include <QRandomGenerator>
#include <QLabel>
#include <QMessageBox>
#include <QDir>
#include <QtWidgets/qapplication.h>
#include <qfiledialog.h>


//돌 착수(1=백, 0=흑)
void newgame::setStoneCarving(int x, int y, int color, int size)
{
    QPushButton *dol = new QPushButton(this);
    dol->setGeometry(x, y, 60, 60);
    dol->setStyleSheet(
        QString("background-color: %1; border-radius: 18px; border: 1px solid gray;")
            .arg(color == 0 ? "black" : "white")
        );

    color == 0 ? data->addScoreBlack(1) : data->addScoreWhite(1);

    QWidget *cell = board[x][y];
    QPoint pos = cell->mapTo(this, QPoint(0, 0));
    dol->move(pos.x(), pos.y());
    dol->show();
    dol->setEnabled(false);
    data->setBoardRock(x, y, color);


    // 돌 저장
    if (stoneButtons[x][y]) {
        delete stoneButtons[x][y];
    }
    stoneButtons[x][y] = dol;

    lastX = x;
    lastY = y;
}


//장애물 랜덤 착수
void newgame::setObstacle(int size, int obstacleCount)
{
    QSet<QPair<int, int>> usedPositions;

    for(int i = 0; i < obstacleCount / 2;)  // 한 쌍씩 배치
    {
        int randX = QRandomGenerator::global()->bounded(size);
        int randY = QRandomGenerator::global()->bounded(size);

        int mirrorX = size - 1 - randX;

        QPair<int, int> pos1(randX, randY);
        QPair<int, int> pos2(mirrorX, randY);

        // 중복 또는 이미 돌이 놓인 위치 방지
        if (!usedPositions.contains(pos1) &&
            !usedPositions.contains(pos2) &&
            data->getBoardRock(randY, randX) == -1 &&
            data->getBoardRock(randY, mirrorX) == -1)
        {
            usedPositions.insert(pos1);
            usedPositions.insert(pos2);

            // 장애물 표시: (y, x)
            QPushButton* btn1 = board[randY][randX];
            QPushButton* btn2 = board[randY][mirrorX];

            btn1->setStyleSheet("background-color: blue; border: 1px solid gray;");
            btn2->setStyleSheet("background-color: blue; border: 1px solid gray;");
            btn1->setEnabled(false);
            btn2->setEnabled(false);

            data->setBoardRock(randY, randX, -2);
            data->setBoardRock(randY, mirrorX, -2);

            ++i;
        }
    }
    //저장

}



//뒤집기
void newgame::flipStone(int x, int y, int color) {
    const int dx[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };
    const int dy[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    int opponent = (color == 1) ? 0 : 1;

    for (int dir = 0; dir < 8; ++dir) {
        QVector<QPair<int, int>> toFlip;
        int ni = x + dx[dir];
        int nj = y + dy[dir];

        while (ni >= 0 && ni < board.size() && nj >= 0 && nj < board.size()) {
            int state = data->getBoardRock(ni, nj);
            if (state == opponent) {
                toFlip.append({ni, nj});
            } else if (state == color) {
                for (const auto& pos : toFlip) {
                    int i = pos.first;
                    int j = pos.second;
                    data->setBoardRock(i, j, color);

                    // 실제 화면에 보이는 돌 버튼 색상 변경
                    QPushButton* dol = stoneButtons[i][j];
                    if (dol) {
                        QString colorStr = (color == 0) ? "black" : "white";
                        dol->setStyleSheet(
                            QString("background-color: %1; border-radius: 18px; border: 1px solid gray;")
                                .arg(colorStr)
                            );
                        dol->update();  // UI 즉시 반영
                    }

                    // 점수 갱신
                    if (color == 0) {
                        data->addScoreBlack(1);
                        data->addScoreWhite(-1);
                    } else {
                        data->addScoreWhite(1);
                        data->addScoreBlack(-1);
                    }
                }
                break;
            } else {
                break;
            }

            ni += dx[dir];
            nj += dy[dir];
        }
    }
}






//착수 가능여부
bool newgame::isCaving(int x, int y, int color) {
    if (data->getBoardRock(x, y) != -1) return false; // 빈 칸 아니면 착수 불가

    const int dx[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };
    const int dy[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };
    int opponent = (color == 1) ? 0 : 1;

    for (int dir = 0; dir < 8; ++dir) {
        int ni = x + dx[dir];
        int nj = y + dy[dir];
        bool foundOpponent = false;

        while (ni >= 0 && ni < board.size() && nj >= 0 && nj < board.size()) {
            int state = data->getBoardRock(ni, nj);
            if (state == opponent) {
                foundOpponent = true;
            } else if (state == color && foundOpponent) {
                return true;
            } else {
                break;
            }
            ni += dx[dir];
            nj += dy[dir];
        }
    }

    return false;
}

//착수 가능여부
bool newgame::onPass(int color) {
    int size = board.size();
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (isCaving(i, j, color))
                return true;
    return false;
}

//착수 가능한 곳 표시
void newgame::updateIsCaving(int color) {
    int size = board.size();
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            QPushButton* btn = board[i][j];
            int state = data->getBoardRock(i, j);
            if (state == -2) continue;

            if (state == -1 && isCaving(i, j, color)) {
                btn->setStyleSheet("background-color: lightgreen; border: 1px solid gray;");
            } else {
                btn->setStyleSheet("background-color: #EEE; border: 1px solid gray;");
            }
        }
    }
}




//초기 착수 상태
void newgame::baseSetting(int size)
{
    if(size == 6)
    {
        setStoneCarving(2, 2, 1, size);
        setStoneCarving(3, 3, 1, size);
        setStoneCarving(2, 3, 0, size);
        setStoneCarving(3, 2, 0, size);
        qDebug("size: %d", size);

    }
    else if(size == 8)
    {
        setStoneCarving(3, 3, 1, size);
        setStoneCarving(4, 4, 1, size);
        setStoneCarving(3, 4, 0, size);
        setStoneCarving(4, 3, 0, size);
        qDebug("size: %d", size);
    }
    else if(size == 10)
    {
        setStoneCarving(4, 4, 1, size);
        setStoneCarving(5, 5, 1, size);
        setStoneCarving(4, 5, 0, size);
        setStoneCarving(5, 4, 0, size);
        qDebug("size: %d", size);
    }
    else if(size == 12)
    {
        setStoneCarving(5, 5, 1, size);
        setStoneCarving(6, 6, 1, size);
        setStoneCarving(5, 6, 0, size);
        setStoneCarving(6, 5, 0, size);
        qDebug("size: %d", size);
    }
}

//정보 최신화
void newgame::updateInfo() {
    sideText1->setText("턴 횟수 : " + QString::number(data->getTurnCount()));
    QString currentPlayer = (data->getPlayer() == 1) ? "백돌" : "흑돌";
    playerLabel->setText("현재 턴: " + currentPlayer);
    whiteCnt->setText("백돌 : " + QString::number(data->getScoreWhite()));
    blackCnt->setText("흑돌 : " + QString::number(data->getScoreBlack()));
    now_btn->setStyleSheet(
        QString("background-color: %1; border-radius: 18px; border: 1px solid gray;")
            .arg(data->getPlayer() == 0 ? "black" : "white")
        );
}

//게임 저장(히스토리와 게임 정보 같이 저장)
void newgame::onGameSave(int blackTime, int whiteTime, int case_, QString name) {
    saveData.boardSize = board.size();

    GameState currentState;
    currentState.boardState = data->getBoardState();
    currentState.turnCount = data->getTurnCount();
    currentState.currentPlayer = data->getPlayer();
    currentState.scoreBlack = data->getScoreBlack();
    currentState.scoreWhite = data->getScoreWhite();
    currentState.blackTime = blackTime;
    currentState.whiteTime = whiteTime;
    currentState.addTime = addTime;
    currentState.time = time;
    currentState.isTimer = isTimer;
    currentState.lastMovePosition = QPoint(lastX, lastY);

    saveData.history.append(currentState);
    qDebug() << "저장된 턴 수: " << saveData.history.size();

    QString path = QDir::homePath() + "/" + name +".dat";
    bool success = saveGameToFile(path, saveData);
    if (!success)
        qDebug() << "저장 실패ㅠㅠ - 경로:" << path;
    else
        qDebug() << "저장 성공!! - 경로:" << path;
}

void newgame::onGameingSave(int blackTime, int whiteTime, int case_, QString name) {
    saveData.boardSize = board.size();

    GameState currentState;
    currentState.boardState = data->getBoardState();
    currentState.turnCount = data->getTurnCount();
    currentState.currentPlayer = data->getPlayer();
    currentState.scoreBlack = data->getScoreBlack();
    currentState.scoreWhite = data->getScoreWhite();
    currentState.blackTime = blackTime;
    currentState.whiteTime = whiteTime;
    currentState.addTime = addTime;
    currentState.time = time;
    currentState.isTimer = isTimer;
    currentState.lastMovePosition = QPoint(lastX, lastY);

    qDebug() << "저장된 턴 수: " << saveData.history.size();

    QString path = QDir::homePath() + "/" + name +".dat";
    bool success = saveGameToFile(path, saveData);
    if (!success)
        qDebug() << "저장 실패ㅠㅠ - 경로:" << path;
    else
        qDebug() << "저장 성공!! - 경로:" << path;
}


//턴마다 저장
void newgame::saveTurnStateToHistory(int blackTime, int whiteTime) {
    GameState currentState;
    currentState.boardState = data->getBoardState();
    currentState.turnCount = data->getTurnCount();
    currentState.currentPlayer = data->getPlayer();
    currentState.scoreBlack = data->getScoreBlack();
    currentState.scoreWhite = data->getScoreWhite();
    currentState.blackTime = blackTime;
    currentState.whiteTime = whiteTime;
    currentState.addTime = addTime;
    currentState.time = time;
    currentState.isTimer = isTimer;
    currentState.lastMovePosition = QPoint(lastY, lastX);

    saveData.history.append(currentState);  // turn마다 저장
    qDebug() << "저장된 턴 수: " << saveData.history.size() << currentState.lastMovePosition;

}

void newgame::onLoad() {
    QString filePath = QFileDialog::getOpenFileName(
        this, "저장된 게임 불러오기", QDir::homePath(), "*.dat");
    if (filePath.isEmpty()) return;

    GameSave loaded;
    if (!loadGameFromFile(filePath, loaded)) {
        QMessageBox::warning(this, "불러오기 실패", "파일을 읽을 수 없습니다.");
        return;
    }

    saveData = loaded;  // 새로 읽은 기록으로 덮어쓰기!

    // 마지막 기록 가져오기
    if (saveData.history.isEmpty()) return;
    const GameState &last = saveData.history.last();

    // 상태 복원
    data->boardStateData = last.boardState;
    data->setTurnCount(last.turnCount);
    data->setPlayer(last.currentPlayer);
    data->setScoreBlack(last.scoreBlack);
    data->setScoreWhite(last.scoreWhite);
    lastX = last.lastMovePosition.x();
    lastY = last.lastMovePosition.y();

    if (isTimer) {
        blackTimer->setRemainTime(last.blackTime);
        whiteTimer->setRemainTime(last.whiteTime);
        if (data->getPlayer() == 0) blackTimer->start();
        else whiteTimer->start();
    }

    // UI 재구성
    refreshUIFromBoard();

    QMessageBox::information(this, "불러오기 완료", "게임 상태를 불러왔습니다.");
}

void newgame::refreshUIFromBoard() {

    // 기존 버튼 모두 제거
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (stoneButtons[i][j]) {
                delete stoneButtons[i][j];
                stoneButtons[i][j] = nullptr;
            }
        }
    }

    // 새 버튼 생성 및 UI 갱신
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int val = data->getBoardRock(i, j);
            if (val == 0 || val == 1) {
                setStoneCarving(i, j, val, size);  // 이 함수 내에서 stoneButtons[i][j]에 버튼 포인터 할당 꼭 확인
            } else {
                updateCellUI(i, j, val); // val에 따라 다르게 처리
            }
        }
    }

    updateInfo();
    updateIsCaving(data->getPlayer());
}



//턴 전환시 타이머 종료 및 시작 기능
void newgame::switchTurn()
{
    if (data->getPlayer() == 1) {
        whiteTimer->stop();
        blackTimer->start();
        whiteTimer->moveMade();
    } else {
        blackTimer->stop();
        whiteTimer->start();
        blackTimer->moveMade();
    }
}


//이전 턴으로 되돌리기
void newgame::undoMove(int boardSize) {
    if (saveData.history.size() < 2) {
        qDebug() << "놓을수없음. 최소 사이즈 미달";
        return;
    }

    saveData.history.removeLast(); // 현재 상태 삭제
    GameState prevState = saveData.history.last(); // 되돌릴 상태 가져오기

    // 상태 복원
    data->boardStateData = prevState.boardState;
    data->setTurnCount(prevState.turnCount);
    data->setPlayer(prevState.currentPlayer);
    data->setScoreBlack(prevState.scoreBlack);
    data->setScoreWhite(prevState.scoreWhite);
    lastX = prevState.lastMovePosition.x();
    lastY = prevState.lastMovePosition.y();

    if(isTimer == true){
        blackTimer->setRemainTime(prevState.blackTime);
        whiteTimer->setRemainTime(prevState.whiteTime);
    }


    // 돌 UI 초기화
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (stoneButtons[i][j]) {
                delete stoneButtons[i][j];
                stoneButtons[i][j] = nullptr;
            }
        }
    }

    // 돌 다시 그리기
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            int val = data->getBoardRock(i, j);
            if (val == 0 || val == 1) {
                setStoneCarving(i, j, val, boardSize);  // 돌만 버튼으로 다시 생성
            } else {
                updateCellUI(i, j, val);  // 장애물(-2)이나 빈 칸(-1) 처리
            }
        }
    }


    updateInfo();
    updateIsCaving(data->getPlayer());
    qDebug() << "턴: " << data->getTurnCount() << " 저장된 history 수: " << saveData.history.size();

}


void newgame::updateCellUI(int i, int j, int stone)
{
    QPushButton* btn = board[i][j];

    if (stone == -2) {
        // 장애물
        btn->setStyleSheet("background-color: blue; border: 1px solid gray;");
        btn->setEnabled(false);
        btn->setText("");
    }
    else if (stone == -1) {
        // 빈 칸
        btn->setStyleSheet("background-color: #EEE; border: 1px solid gray;");
        btn->setEnabled(true);
        btn->setText("");
    }
}




newgame::newgame(int size, int firstRock, bool isTimer, int time, int addTime, int case_, QWidget *parent, database *data, const GameSave* saveData)
    : QWidget(parent), data(data), saveData(saveData ? *saveData : GameSave())
{
    this->isTimer = isTimer;
    this->time = time;
    this->addTime = addTime;
    if(case_ == 0) //새게임용
    {
        if(isTimer == true)
        {
            whiteTimer = new onTimer(time, addTime, this);
            blackTimer = new onTimer(time, addTime, this);

            connect(whiteTimer, &onTimer::timeChanged, this, [=](int sec) {
                qDebug() << "White time left:" << sec;
                timeLabel->setText(QString("백돌 남은 시간 : %1초").arg(sec));
            });

            connect(whiteTimer, &onTimer::timeExpired, this, [=]() {
                qDebug() << "White time expired!";
                QMessageBox::information(this, "시간 종료", "백돌 시간 초과! 흑돌 승리!");
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "리플레이", "게임을 저장하시겠습니까?",
                                              QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    onGameSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 0, "replay_save");
                    this->close();
                }
                whiteTimer->stop();
                blackTimer->stop();
                this->close();
                data->setIsContinue(false);
            });

            connect(blackTimer, &onTimer::timeChanged, this, [=](int sec) {
                qDebug() << "Black time left:" << sec;
                timeLabel->setText(QString("흑돌 남은 시간 : %1초").arg(sec));
            });

            connect(blackTimer, &onTimer::timeExpired, this, [=]() {
                qDebug() << "Black time expired!";
                QMessageBox::information(this, "시간 종료", "흑돌 시간 초과! 백돌 승리!");

                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "리플레이", "게임을 저장하시겠습니까?",
                                              QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    onGameingSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 0, "save");
                    this->close();
                }
                whiteTimer->stop();
                blackTimer->stop();
                this->close();
                data->setIsContinue(false);

            });

            timeLabel = new QLabel("남은 시간 : 초", this);
        }


        // 전체 수평 레이아웃 생성
        QHBoxLayout *mainLayout = new QHBoxLayout(this);

        // 기존 그리드 레이아웃
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);

        now_btn = new QPushButton(this);
        now_btn->setFixedSize(60, 60);
        now_btn->setEnabled(false);

        // 텍스트 라벨 생성
        sideText1 = new QLabel("턴 횟수 : " + QString::number(data->getTurnCount()), this);

        QString currentPlayer = (data->getPlayer() == 1) ? "백돌" : "흑돌";

        whiteCnt = new QLabel("백돌 : " + QString::number(data->getScoreWhite()), this);
        blackCnt = new QLabel("흑돌 : " + QString::number(data->getScoreBlack()), this);

        playerLabel = new QLabel("현재 턴: " + currentPlayer, this);

        Undobtn = new QPushButton(this);
        Undobtn->setText("되돌리기");

        Savebtn = new QPushButton(this);
        Savebtn->setText("저장하기");

        MoveMainbtn = new QPushButton(this);
        MoveMainbtn->setText("메인화면");


        playerLabel->setFixedWidth(150);
        sideText1->setFixedWidth(150);
        whiteCnt->setFixedWidth(150);
        blackCnt->setFixedWidth(150);
        Savebtn->setFixedSize(150, 30);
        Undobtn->setFixedSize(150, 30);
        MoveMainbtn->setFixedSize(150, 30);

        QVBoxLayout *sideLayout = new QVBoxLayout();
        sideLayout->addWidget(now_btn);

        if(isTimer == true)
            sideLayout->addWidget(timeLabel);

        sideLayout->addWidget(sideText1);
        sideLayout->addWidget(playerLabel);
        sideLayout->addWidget(whiteCnt);
        sideLayout->addWidget(blackCnt);
        sideLayout->addWidget(Undobtn);
        sideLayout->addWidget(Savebtn);
        sideLayout->addWidget(MoveMainbtn);
        sideLayout->addStretch(0);

        // 텍스트와 그리드 추가
        mainLayout->addLayout(gridLayout);
        mainLayout->addLayout(sideLayout);

        board.resize(size);

        for (int i = 0; i < size; ++i) {
            board[i].resize(size);
            for (int j = 0; j < size; ++j) {
                QPushButton *btn = new QPushButton(this);
                btn->setFixedSize(60, 60);
                btn->setStyleSheet("background-color: #EEE; border: 1px solid gray;");
                gridLayout->addWidget(btn, i, j);
                board[i][j] = btn;

                disconnect(MoveMainbtn, &QPushButton::clicked, nullptr, nullptr);
                connect(MoveMainbtn, &QPushButton::clicked, this, [=]() {
                    this->hide();
                    data->setIsContinue(true);
                    if(isTimer == true)
                    {
                        whiteTimer->stop();
                        blackTimer->stop();
                    }
                    //타이머도 멈춤
                });

                disconnect(Savebtn, &QPushButton::clicked, nullptr, nullptr);
                connect(Savebtn, &QPushButton::clicked, this, [=]() {
                    //저장
                    if(isTimer == true)
                    {
                        whiteTimer->stop();
                        blackTimer->stop();
                        onGameingSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 0, "save");
                    }else{
                        onGameingSave(0,0,0,"save");
                    }
                    data->setIsContinue(false);
                    this->close();
                });

                disconnect(Undobtn, &QPushButton::clicked, nullptr, nullptr);
                connect(Undobtn, &QPushButton::clicked, this, [=]() {
                    if(isTimer == true)
                        switchTurn();
                    if(data->getPlayer() == 0)
                        data->setPlayer(1);
                    else
                        data->setPlayer(0);

                    undoMove(size);

                });

                // 보드 클릭
                connect(btn, &QPushButton::clicked, this, [=]() {
                    int color = data->getPlayer();

                    if (!isCaving(i, j, color)) return;

                    setStoneCarving(i, j, color, size);

                    flipStone(i, j, color);
                    data->addTurnCount(1);

                    //저장
                    if(isTimer == true)
                    {
                        switchTurn();
                        saveTurnStateToHistory(blackTimer->getRemainTime(), whiteTimer->getRemainTime());
                    }else{
                        saveTurnStateToHistory(0,0);
                    }

                    // 다음 턴 플레이어

                    int nextPlayer = 1 - color;

                    // 착수 가능 여부 확인
                    if (onPass(nextPlayer)) {
                        data->setPlayer(nextPlayer);
                    } else if (onPass(color)) {
                        QMessageBox::information(this, "턴 패스", "상대방이 둘 곳이 없습니다. 턴을 유지합니다.");

                    } else {
                        // 양쪽 모두 둘 곳 없음 = 게임 종료'
                        QString BlackCnt = QString::number(data->getScoreBlack());
                        QString WhiteCnt = QString::number(data->getScoreWhite());
                        updateInfo();
                        if(data->getScoreWhite() < data->getScoreBlack()){
                            QMessageBox::information(this, "흑돌 승", "흑돌 승\n흑돌 : " + BlackCnt + " 백돌 : " + WhiteCnt);
                        }else if(data->getScoreWhite() > data->getScoreBlack()){
                            QMessageBox::information(this, "백돌 승", "백돌 승\n흑돌 : " + BlackCnt + " 백돌 : " + WhiteCnt);
                        }else{
                            QMessageBox::information(this, "무승부", "무승부!\n흑돌 : " + BlackCnt + " 백돌 : " + WhiteCnt);
                        }

                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, "리플레이", "게임을 저장하시겠습니까?",
                                                      QMessageBox::Yes | QMessageBox::No);

                        if (reply == QMessageBox::Yes) {
                            if(isTimer == true)
                                onGameSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 0, "replay_save");
                            else
                                onGameSave(0,0,0,"replay_save");
                        }
                        if(isTimer == true)
                        {
                            whiteTimer->stop();
                            blackTimer->stop();
                        }
                        this->close();
                        data->setIsContinue(false);

                    }

                    updateInfo();
                    updateIsCaving(data->getPlayer());
                });
            }
        }

        //그리드 버튼 완성후 초기 세팅을 위해 조금 지연 시키기
        QTimer::singleShot(0, this, [=]() {
            baseSetting(size);
            setObstacle(size, data->getObstacleCount());

            if(isTimer == true)
                saveTurnStateToHistory(blackTimer->getRemainTime(), whiteTimer->getRemainTime());
            else
                saveTurnStateToHistory(0,0);
            updateInfo();
            updateIsCaving(data->getPlayer());
        });
    }

    else if(case_ == 1) //저장용
    {
        if (this->saveData.history.isEmpty()) {
            QMessageBox::warning(this, "불러오기 오류", "저장된 게임 상태가 없습니다.");
            return;
        }
        isTimer = this->saveData.history.last().isTimer;
        whiteTime = this->saveData.history.last().whiteTime;
        blackTime = this->saveData.history.last().blackTime;
        addTime = this->saveData.history.last().addTime;
        qDebug() << "isTimer:" << isTimer;

        if(isTimer == true)
        {

            whiteTimer = new onTimer(whiteTime, addTime, this);
            blackTimer = new onTimer(blackTime, addTime, this);

            if(data->getPlayer() == 0)
                blackTimer->start();
            else
                whiteTimer->start();

            connect(whiteTimer, &onTimer::timeChanged, this, [=](int sec) {
                qDebug() << "White time left:" << sec;
                timeLabel->setText(QString("백돌 남은 시간 : %1초").arg(sec));
            });

            connect(whiteTimer, &onTimer::timeExpired, this, [=]() {
                qDebug() << "White time expired!";
                QMessageBox::information(this, "시간 종료", "백돌 시간 초과! 흑돌 승리!");
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "리플레이", "게임을 저장하시겠습니까?",
                                              QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    onGameSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 1, "replay_save");
                    this->close();
                }
                whiteTimer->stop();
                blackTimer->stop();
                this->close();
                data->setIsContinue(false);
            });

            connect(blackTimer, &onTimer::timeChanged, this, [=](int sec) {
                qDebug() << "Black time left:" << sec;
                timeLabel->setText(QString("흑돌 남은 시간 : %1초").arg(sec));
            });

            connect(blackTimer, &onTimer::timeExpired, this, [=]() {
                qDebug() << "Black time expired!";
                QMessageBox::information(this, "시간 종료", "흑돌 시간 초과! 백돌 승리!");

                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "리플레이", "게임을 저장하시겠습니까?",
                                              QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    onGameingSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 1,"save");
                    this->close();
                }
                whiteTimer->stop();
                blackTimer->stop();
                this->close();
                data->setIsContinue(false);

            });

            timeLabel = new QLabel("남은 시간 : 초", this);
        }

        // 전체 수평 레이아웃 생성
        QHBoxLayout *mainLayout = new QHBoxLayout(this);

        // 기존 그리드 레이아웃
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);

        now_btn = new QPushButton(this);
        now_btn->setFixedSize(60, 60);
        now_btn->setEnabled(false);

        this->data->setTurnCount(this->saveData.history.last().turnCount);
        // 텍스트 라벨 생성
        sideText1 = new QLabel("턴 횟수 : " + QString::number(data->getTurnCount()), this);

        QString currentPlayer = (data->getPlayer() == 1) ? "백돌" : "흑돌";

        whiteCnt = new QLabel("백돌 : " + QString::number(data->getScoreWhite()), this);
        blackCnt = new QLabel("흑돌 : " + QString::number(data->getScoreBlack()), this);

        playerLabel = new QLabel("현재 턴: " + currentPlayer, this);

        Undobtn = new QPushButton(this);
        Undobtn->setText("되돌리기");

        Savebtn = new QPushButton(this);
        Savebtn->setText("저장하기");

        MoveMainbtn = new QPushButton(this);
        MoveMainbtn->setText("메인화면");


        playerLabel->setFixedWidth(150);
        sideText1->setFixedWidth(150);
        whiteCnt->setFixedWidth(150);
        blackCnt->setFixedWidth(150);
        Savebtn->setFixedSize(150, 30);
        Undobtn->setFixedSize(150, 30);
        MoveMainbtn->setFixedSize(150, 30);

        QVBoxLayout *sideLayout = new QVBoxLayout();
        sideLayout->addWidget(now_btn);

        if(isTimer == true)
            sideLayout->addWidget(timeLabel);

        sideLayout->addWidget(sideText1);
        sideLayout->addWidget(playerLabel);
        sideLayout->addWidget(whiteCnt);
        sideLayout->addWidget(blackCnt);
        sideLayout->addWidget(Undobtn);
        sideLayout->addWidget(Savebtn);
        sideLayout->addWidget(MoveMainbtn);
        sideLayout->addStretch(0);

        // 텍스트와 그리드 추가
        mainLayout->addLayout(gridLayout);
        mainLayout->addLayout(sideLayout);

        board.resize(size);

        for (int i = 0; i < size; ++i) {
            board[i].resize(size);
            for (int j = 0; j < size; ++j) {
                QPushButton *btn = new QPushButton(this);
                btn->setFixedSize(60, 60);
                btn->setStyleSheet("background-color: #EEE; border: 1px solid gray;");
                gridLayout->addWidget(btn, i, j);
                board[i][j] = btn;

                disconnect(MoveMainbtn, &QPushButton::clicked, nullptr, nullptr);
                connect(MoveMainbtn, &QPushButton::clicked, this, [=]() {
                    this->hide();
                    data->setIsContinue(true);
                    if(isTimer == true)
                    {
                        whiteTimer->stop();
                        blackTimer->stop();
                    }
                    //타이머도 멈춤
                });

                disconnect(Savebtn, &QPushButton::clicked, nullptr, nullptr);
                connect(Savebtn, &QPushButton::clicked, this, [=]() {
                    //저장
                    if(isTimer == true)
                    {
                        whiteTimer->stop();
                        blackTimer->stop();
                        onGameingSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 1, "save");
                    }else{
                        onGameingSave(0,0,1,"save");
                    }
                    data->setIsContinue(false);
                    this->close();
                });

                disconnect(Undobtn, &QPushButton::clicked, nullptr, nullptr);
                connect(Undobtn, &QPushButton::clicked, this, [=]() {
                    undoMove(size);
                });

                // 보드 클릭
                connect(btn, &QPushButton::clicked, this, [=]() {
                    int color = data->getPlayer();

                    if (!isCaving(i, j, color)) return;

                    setStoneCarving(i, j, color, size);

                    flipStone(i, j, color);
                    data->addTurnCount(1);

                    //저장
                    if(isTimer == true)
                        saveTurnStateToHistory(blackTimer->getRemainTime(), whiteTimer->getRemainTime());
                    else
                        saveTurnStateToHistory(0,0);

                    if(isTimer == true)
                        switchTurn();

                    // 다음 턴 플레이어
                    int nextPlayer = 1 - color;

                    // 착수 가능 여부 확인
                    if (onPass(nextPlayer)) {
                        data->setPlayer(nextPlayer);
                    } else if (onPass(color)) {
                        QMessageBox::information(this, "턴 패스", "상대방이 둘 곳이 없습니다. 턴을 유지합니다.");

                    } else {
                        // 양쪽 모두 둘 곳 없음 = 게임 종료'
                        QString BlackCnt = QString::number(data->getScoreBlack());
                        QString WhiteCnt = QString::number(data->getScoreWhite());
                        updateInfo();
                        if(data->getScoreWhite() < data->getScoreBlack()){
                            QMessageBox::information(this, "흑돌 승", "흑돌 승\n흑돌 : " + BlackCnt + " 백돌 : " + WhiteCnt);
                        }else if(data->getScoreWhite() > data->getScoreBlack()){
                            QMessageBox::information(this, "백돌 승", "백돌 승\n흑돌 : " + BlackCnt + " 백돌 : " + WhiteCnt);
                        }else{
                            QMessageBox::information(this, "무승부", "무승부!\n흑돌 : " + BlackCnt + " 백돌 : " + WhiteCnt);
                        }

                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, "리플레이", "게임을 저장하시겠습니까?",
                                                      QMessageBox::Yes | QMessageBox::No);

                        if (reply == QMessageBox::Yes) {
                            if(isTimer == true)
                                onGameSave(blackTimer->getRemainTime(), whiteTimer->getRemainTime(), 1, "replay_save");
                            else
                                onGameSave(0,0,1,"replay_save");
                            this->close();
                        }
                        if(isTimer == true)
                        {
                            whiteTimer->stop();
                            blackTimer->stop();
                        }
                        this->close();
                        data->setIsContinue(false);

                    }

                    updateInfo();
                    updateIsCaving(data->getPlayer());
                });
            }
        }

        //그리드 버튼 완성후 초기 세팅을 위해 조금 지연 시키기
        QTimer::singleShot(0, this, [=]() {
            if (this->saveData.history.isEmpty()) {
                QMessageBox::warning(this, "불러오기 오류", "저장된 게임 상태가 없습니다.");
                return;
            }

            // boardState 적용
            for (int i = 0; i < this->saveData.boardSize; ++i) {
                for (int j = 0; j < this->saveData.boardSize; ++j) {
                    this->data->setBoardRock(i, j, this->saveData.history.last().boardState[i][j]);
                }
            }

            // UI 갱신
            this->updateInfo(); // 점수/턴 정보 등 텍스트 갱신
            this->updateIsCaving(this->data->getPlayer()); // 착수 가능 위치 표시


            // 돌 UI 초기화
            for (int i = 0; i < this->saveData.boardSize; ++i) {
                for (int j = 0; j < this->saveData.boardSize; ++j) {
                    if (stoneButtons[i][j]) {
                        delete stoneButtons[i][j];
                        stoneButtons[i][j] = nullptr;
                    }
                }
            }

            for (int i = 0; i < this->saveData.boardSize; ++i) {
                for (int j = 0; j < this->saveData.boardSize; ++j) {
                    int val = data->getBoardRock(i, j);
                    if (val == 0 || val == 1) {
                        setStoneCarving(i, j, val, this->saveData.boardSize);  // 돌만 버튼으로 다시 생성
                    } else {
                        updateCellUI(i, j, val);  // 장애물(-2)이나 빈 칸(-1) 처리
                    }
                }
            }

            updateInfo();
            updateIsCaving(data->getPlayer());
        });
    }
}


newgame::~newgame()
{

}

