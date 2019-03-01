#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMdiSubWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionKoniec_triggered();
    void on_actionOprogramie_triggered();
    void on_actionUzytkownicy_triggered();
    void on_actionProdukty_triggered();
    void on_actionKontrahenci_triggered();
    void on_actionPrzyjeciaPZ_triggered();
    void on_actionWydaniaWZ_triggered();

    void openWydanie(int &id);
    void openPrzyjecie(int &id);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;

    void writeSettings();
    void readSettings();
    bool createOpenDatabase();
};

#endif // MAINWINDOW_H
