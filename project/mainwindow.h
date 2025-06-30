#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include "newgame.h"
#include "setting.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event) override;

    newgame *ingame;

private slots:
    void on_btn_newgame_clicked();

    void on_quit_clicked();

    void on_btn_newgame_2_clicked();

    void on_btn_newgame_3_clicked();

    void on_btnComeSave_clicked();

private:
    Ui::MainWindow *ui;
    setting *setting;

    bool isReplay = false;
};
#endif // MAINWINDOW_H
