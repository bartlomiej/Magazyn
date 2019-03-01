#include "logowanie.h"
#include "ui_logowanie.h"

Logowanie::Logowanie(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Logowanie)
{
    ui->setupUi(this);

}

Logowanie::~Logowanie()
{
    delete ui;
}

void Logowanie::on_pbAnuluj_clicked()
{
    emit sig_AnulujLogowanie();
    close();
}

void Logowanie::on_pbZaloguj_clicked()
{
    bool ret = false;
    QByteArray result = hash->hash(ui->leHaslo->text().toLatin1(),QCryptographicHash::Md5);
    QString hashResult(result.toHex());
    QSqlQuery query;
    query.prepare("SELECT [IdUzytkownika],[Nazwisko],[Imie],[Login],[Haslo] FROM [Uzytkownicy] "
                  "WHERE [Login] = :login AND [Haslo] = :password");

    query.bindValue(":login",ui->leLogin->text());
    query.bindValue(":password",hashResult);
    if (!query.exec())
    {
        QMessageBox::critical(this, tr("Błąd logowania"),
                                       query.lastError().text(),
                                       QMessageBox::Ok);
        return;
    }

    while(query.next()) {
        ret = true;
    }
    if(ret)
    {
        ui->leLogin->setStyleSheet("");
        ui->leHaslo->setStyleSheet("");
        emit sig_LogowaniePoprawne();
    }else{
        ui->leLogin->setStyleSheet("border: 1px solid red");
        ui->leHaslo->setStyleSheet("border: 1px solid red");
    }
}
