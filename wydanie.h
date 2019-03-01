#ifndef WYDANIE_H
#define WYDANIE_H

#include <QObject>
#include <QWidget>
#include <QModelIndex>
#include <QToolButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QTableView>
#include <QSettings>
#include <QDebug>
#include <QCloseEvent>
#include <QApplication>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTimeEdit>
#include <QMessageBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

class Wydanie : public QWidget
{
    Q_OBJECT
public:
    explicit Wydanie(int &_id, QWidget *parent = 0);
    int Id(){ return id; }

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sig_close();

private slots:
    void slot_New();
    void slot_Close();
    void slot_Delete();
    bool slot_Save();

    void slot_PozycjeAdd();
    void slot_PozycjeDel();
    void setDirty(){
        setWindowModified(true);
    }
    void setTitle(){
        setWindowTitle(leNumer->text() + "[*]");
    }

private:
    int id;

    QToolBar *toolBar;
    QToolButton *actionNew;
    QToolButton *actionSave;
    QToolButton *actionDelete;
    QToolButton *actionClose;

    QToolButton *actionDodajPozycje;
    QToolButton *actionUsunPozycje;

    QSqlRelationalTableModel *model;
    QTableView *view;
    QSqlQuery query;

    QVBoxLayout *vblGlowny;
    QGroupBox *gbDanePodstawowe;
    QGridLayout *gridLayout;
    QHBoxLayout *hblDanePodstawowe;
    QSpacerItem *siDanePodstawowe;

    QPushButton *pbPozycjeAdd;
    QPushButton *pbPozycjeDel;
    QSpacerItem *siPozycje;
    QHBoxLayout *hblPozycje;

    QLabel *lNumer;
    QLineEdit *leNumer;
    QLabel *lRodzajObrotu;
    QLineEdit *leRodzajObrotu;
    QLabel *lDataObrotu;
    QDateTimeEdit *dteDataObrotu;
    QLabel *lKontrahent;
    QComboBox *cbKontrahent;
    QLabel *lPozycje;

    void writeSettings();
    void readSettings();
    void createWidgets();
    void createLayout();
    void createConnections();
    bool checkRequiredFields();
    bool okToClearData();
    void loadData();
};

#endif // WYDANIE_H
