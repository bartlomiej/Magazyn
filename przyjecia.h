#ifndef PRZYJECIA_H
#define PRZYJECIA_H

#include <QWidget>
#include <QCloseEvent>
#include <QSqlRelationalDelegate>
#include <QSqlQuery>
#include <QToolBar>
#include <QTableView>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <QHeaderView>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QSortFilterProxyModel>
#include <QToolButton>

class Przyjecia : public QWidget
{
    Q_OBJECT

public:
    explicit Przyjecia(QWidget *parent = 0);
    ~Przyjecia();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void slot_New();
    void slot_Edit();
    void slot_Delete();
    void slot_Close();
    void slot_Refresh();
    void slot_view_doubleClicked(const QModelIndex &index);

signals:
    void sig_open(int &id);
    void sig_close();

private:
    QToolBar *toolBar;
    QToolButton *actionNew;
    QToolButton *actionEdit;
    QToolButton *actionDelete;
    QToolButton *actionRefresh;
    QToolButton *actionClose;
    QSqlRelationalTableModel *model;
    QTableView *view;
    QSortFilterProxyModel *proxy;

    QVBoxLayout *vblGlowny;

    void readSettings();
    void writeSettings();
    void createWidgets();
    void createLayout();
    void createConnections();
};

#endif // PRZYJECIA_H
