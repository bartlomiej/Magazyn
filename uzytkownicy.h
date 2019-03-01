#ifndef UZYTKOWNICY_H
#define UZYTKOWNICY_H

#include <QWidget>
#include <QCloseEvent>
#include <QSqlRelationalDelegate>
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

class Uzytkownicy : public QWidget
{
    Q_OBJECT

public:
    explicit Uzytkownicy(QWidget *parent = 0);
    ~Uzytkownicy();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void slot_New();
    void slot_Edit();
    void slot_Delete();
    void slot_Cancel();
    void slot_Close();
    bool slot_Save();
    void setDirty() { setWindowModified(true); }

signals:
    void sig_close();

private:
    enum state {Display = 0, New, Edit, Save, Delete};
    state stan;

    QSqlTableModel model;
    QTableView *view;
    QGridLayout *gridLayout;
    QVBoxLayout *centralLayout;
    QToolBar *toolBar;
    QAction *actionNew;
    QAction *actionEdit;
    QAction *actionSave;
    QAction *actionDelete;
    QAction *actionCancel;
    QAction *actionHelp;
    QAction *actionClose;
    QGroupBox *gbEdit;
    QLabel *lNazwisko;
    QLineEdit *leNazwisko;
    QLabel *lImie;
    QLineEdit *leImie;
    QLabel *lLogin;
    QLineEdit *leLogin;
    QLabel *lHaslo;
    QLineEdit *leHaslo;
    QCryptographicHash *hash;

    QDataWidgetMapper *mapper;
    void readSettings();
    void writeSettings();
    void createWidgets();
    void createLayout();
    void createConnections();
};

class UzytkownicyDelegate : public QSqlRelationalDelegate
{
    Q_OBJECT
public:
    UzytkownicyDelegate(QObject *parent = 0);
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
};
#endif // UZYTKOWNICY_H
