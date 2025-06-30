#include "gamesave.h"
#include <QFile>
#include <QDataStream>
#include <QtCore/qdebug.h>

// GameSave 클래스 멤버 함수로 정의
int GameSave::getBoardSize() const {
    if (!history.isEmpty())
        return history.last().boardState.size();
    return 0;
}

int GameSave::getBoardRock(int x, int y) const {
    if (!history.isEmpty()) {
        const QVector<QVector<int>>& board = history.last().boardState;
        if (x >= 0 && x < board.size() && y >= 0 && y < board[x].size()) {
            return board[x][y];
        }
    }
    return -1; // 오류 값
}

bool saveGameToFile(const QString &filename, const GameSave &saveData) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QDataStream out(&file);
    out << saveData;
    return true;
}

bool loadGameFromFile(const QString &filename, GameSave &saveData) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QDataStream in(&file);
    in >> saveData;

    qDebug() << "History size:" << saveData.history.size();
    for (int i = 0; i < saveData.history.size(); ++i) {
        qDebug() << "Turn" << i
                 << "Board size:" << saveData.history[i].boardState.size()
                 << "Row 0 size:" << (saveData.history[i].boardState.isEmpty() ? -1 : saveData.history[i].boardState[0].size());
    }

    return true;
}
