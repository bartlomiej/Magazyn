#include "ui_mainwindow.h"
#include "kontrahenci.h"
#include "logowanie.h"
#include "mainwindow.h"
#include "produkty.h"
#include "uzytkownicy.h"
#include "przyjecia.h"
#include "wydania.h"
#include "przyjecie.h"
#include "wydanie.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!createOpenDatabase())
    {
        QMessageBox::about(this, tr("Magazyn"),
                           QString("Błąd podczas kreowania bazy danych!\nSkontaktuj się z administratorem."));
        close();

    }
    readSettings();
    Logowanie *login = new Logowanie();
    connect(login,SIGNAL(sig_AnulujLogowanie()),this,SLOT(close()));
    connect(login,SIGNAL(sig_LogowaniePoprawne()),login,SLOT(close()));
    login->setModal(true);
    login->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    writeSettings();
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setIniCodec("UTF-8");
    settings.setFallbacksEnabled(false);
    settings.beginGroup("Magazyn");

    settings.setValue("geometry",saveGeometry());
    settings.setValue("savestate",saveState() );
    settings.setValue("maximized",isMaximized());
    if (!isMaximized()) {
        settings.setValue("pos",pos());
        settings.setValue("size",size());
    }
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setIniCodec("UTF-8");
    settings.setFallbacksEnabled(false);
    settings.beginGroup("Magazyn");

    restoreGeometry(settings.value("geometry",saveGeometry()).toByteArray());
    restoreState(settings.value("savestate",saveState() ).toByteArray());
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size",size() ).toSize());
    if (settings.value("maximized",isMaximized()).toBool())
        showMaximized();
    settings.endGroup();
}

void MainWindow::on_actionKoniec_triggered()
{
    close();
}

bool MainWindow::createOpenDatabase()
{
    bool result = false;
    bool wynik = true;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(qApp->applicationDirPath() + "/magazyn.sqlite");
    if (!db.open())
    {
        qDebug() << "Błąd otwarcia bazy!";
        return -1;
    }else{
        QSqlQuery query(db);
        result = query.exec("CREATE TABLE IF NOT EXISTS [Uzytkownicy] ("
                            "[IdUzytkownika] INTEGER UNIQUE PRIMARY KEY,"
                            "[Nazwisko] TEXT NOT NULL,"
                            "[Imie] TEXT,"
                            "[Login] TEXT UNIQUE NOT NULL,"
                            "[Haslo] TEXT)");
        if (!result)
        {
            qDebug() << "Błąd tworzenia tabeli [Użytkownicy] w bazie danych. " << query.lastError().text();
            wynik = result;
        }

        query.exec("INSERT INTO [Uzytkownicy] ([Nazwisko],[Imie],[Login],[Haslo]) "
                   "VALUES ('admin', 'admin', 'admin', '21232f297a57a5a743894a0e4a801fc3')");

        result = query.exec("CREATE TABLE IF NOT EXISTS [Obroty_magazynowe] ("
                            "[IdObrotu] INTEGER UNIQUE PRIMARY KEY,"
                            "[RodzajObrotu] TEXT NOT NULL,"
                            "[Numer] TEXT NOT NULL,"
                            "[IdKontrahenta] INTEGER NOT NULL,"
                            "[DataObrotu] TEXT NOT NULL)");
        if (!result)
        {
            qDebug() << "Błąd tworzenia tabeli [Obroty_magazynowe] w bazie danych. " << query.lastError().text();
            wynik = result;
        }

        result = query.exec("CREATE TABLE IF NOT EXISTS [Obroty_magazynowe_pozycje] ("
                            "[IdPozycji] INTEGER UNIQUE PRIMARY KEY,"
                            "[IdObrotu] INTEGER NOT NULL,"
                            "[IdProduktu] INTEGER NOT NULL,"
                            "[Ilosc] FLOAT NOT NULL,"
                            "[Cena] FLOAT NOT NULL)");
        if (!result)
        {
            qDebug() << "Błąd tworzenia tabeli [Obroty_magazynowe_pozycje] w bazie danych. " << query.lastError().text();
            wynik = result;
        }

        result = query.exec("CREATE TABLE IF NOT EXISTS [Produkty] ("
                            "[IdProduktu] INTEGER UNIQUE PRIMARY KEY,"
                            "[Indeks] TEXT UNIQUE NOT NULL,"
                            "[Nazwa] TEXT UNIQUE NOT NULL)");

        if (!result)
        {
            qDebug() << "Błąd tworzenia tabeli [Produkty] w bazie danych. " << query.lastError().text();
            wynik = result;
        }

        result = query.exec("CREATE TABLE IF NOT EXISTS [Kontrahenci] ("
                            "[IdKontrahenta] INTEGER UNIQUE PRIMARY KEY,"
                            "[Indeks] TEXT UNIQUE NOT NULL,"
                            "[Nazwa] TEXT UNIQUE NOT NULL)");
        if (!result)
        {
            qDebug() << "Błąd tworzenia tabeli [Kontrahenci] w bazie danych. " << query.lastError().text();
            wynik = result;
        }
    }
    return wynik;
}

void MainWindow::on_actionOprogramie_triggered()
{
    QMessageBox::about(this, tr("Magazyn"),
                       QString("Aplikacja do zarządzania magazynem.\n" + QApplication::applicationName() + " v" + QApplication::applicationVersion()));
}

void MainWindow::on_actionUzytkownicy_triggered()
{
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Uzytkownicy *existing;
        existing = qobject_cast<Uzytkownicy *>(window->widget());
        if (existing)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Uzytkownicy *users = new Uzytkownicy(mdiSW);
    mdiSW->setWindowTitle(tr("Użytkownicy"));
    mdiSW->setWidget(users);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(users,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    users->show();
}

void MainWindow::on_actionProdukty_triggered()
{
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Produkty *existing;
        existing = qobject_cast<Produkty *>(window->widget());
        if (existing)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Produkty *products = new Produkty(mdiSW);
    mdiSW->setWindowTitle(tr("Produkty"));
    mdiSW->setWidget(products);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(products,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    products->show();
}

void MainWindow::on_actionKontrahenci_triggered()
{
    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Kontrahenci *existing;
        existing = qobject_cast<Kontrahenci *>(window->widget());
        if (existing)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Kontrahenci *contractors = new Kontrahenci(mdiSW);
    mdiSW->setWindowTitle(tr("Kontrahenci"));
    mdiSW->setWidget(contractors);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(contractors,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    contractors->show();
}

void MainWindow::on_actionPrzyjeciaPZ_triggered()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());
    settings.setFallbacksEnabled(false);

    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Przyjecia *existing;
        existing = qobject_cast<Przyjecia *>(window->widget());
        if (existing)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Przyjecia *przyjecia = new Przyjecia(mdiSW);
    mdiSW->setWindowTitle(tr("Przyjęcia"));
    mdiSW->setWidget(przyjecia);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(przyjecia,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    connect(przyjecia,SIGNAL(sig_open(int&)),this,SLOT(openPrzyjecie(int&)));
    settings.beginGroup("Przyjecia");
    if (settings.value("max",false).toBool())
        przyjecia->showMaximized();
    else
        przyjecia->show();
    settings.endGroup();
}

void MainWindow::on_actionWydaniaWZ_triggered()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());
    settings.setFallbacksEnabled(false);

    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Wydania *existing;
        existing = qobject_cast<Wydania *>(window->widget());
        if (existing)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Wydania *wydania = new Wydania(mdiSW);
    mdiSW->setWindowTitle(tr("Wydania"));
    mdiSW->setWidget(wydania);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(wydania,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    connect(wydania,SIGNAL(sig_open(int&)),this,SLOT(openWydanie(int&)));
    settings.beginGroup("Wydania");
    if (settings.value("max",false).toBool())
        wydania->showMaximized();
    else
        wydania->show();
    settings.endGroup();
}

void MainWindow::openPrzyjecie(int &id)
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());
    settings.setFallbacksEnabled(false);

    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Przyjecie *existing;
        existing = qobject_cast<Przyjecie *>(window->widget());
        if (existing && existing->Id() == id)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Przyjecie *przyjecie = new Przyjecie(id,mdiSW);
    mdiSW->setWidget(przyjecie);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(przyjecie,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    settings.beginGroup("Przyjecie");
    if (settings.value("max",false).toBool())
        przyjecie->showMaximized();
    else
        przyjecie->show();
    settings.endGroup();
}

void MainWindow::openWydanie(int &id)
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());
    settings.setFallbacksEnabled(false);

    foreach(QMdiSubWindow *window, ui->mdiArea->subWindowList())
    {
        Wydanie *existing;
        existing = qobject_cast<Wydanie *>(window->widget());
        if (existing && existing->Id() == id)
        {
            existing->setFocus();
            return;
        }
    }
    QMdiSubWindow *mdiSW = new QMdiSubWindow(this);
    mdiSW->setAttribute(Qt::WA_DeleteOnClose);
    Wydanie *wydanie = new Wydanie(id,mdiSW);
    mdiSW->setWidget(wydanie);
    ui->mdiArea->addSubWindow(mdiSW);
    connect(wydanie,SIGNAL(sig_close()),mdiSW,SLOT(close()));
    settings.beginGroup("Wydanie");
    if (settings.value("max",false).toBool())
        wydanie->showMaximized();
    else
        wydanie->show();
    settings.endGroup();
}
