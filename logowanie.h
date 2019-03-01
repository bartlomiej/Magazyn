#ifndef LOGOWANIE_H
#define LOGOWANIE_H

#include <QCryptographicHash>
#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

namespace Ui {
class Logowanie;
}

class Logowanie : public QDialog
{
    Q_OBJECT

public:
    explicit Logowanie(QWidget *parent = 0);
    ~Logowanie();

private slots:
    void on_pbAnuluj_clicked();
    void on_pbZaloguj_clicked();

private:
    Ui::Logowanie *ui;
    QCryptographicHash *hash;

signals:
    sig_AnulujLogowanie();
    sig_LogowaniePoprawne();
};

#endif // LOGOWANIE_H
