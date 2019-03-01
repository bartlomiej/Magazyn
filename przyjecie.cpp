#include "przyjecie.h"

Przyjecie::Przyjecie(int &_id, QWidget *parent)
    : QWidget(parent), id(_id)
{
    setAttribute(Qt::WA_DeleteOnClose);
    createWidgets();
    createLayout();
    createConnections();
    loadData();
    setWindowTitle(leNumer->text() + "[*]");
    readSettings();
}

void Przyjecie::loadData()
{
    if (!okToClearData())
        return;
    if (id == 0)
    {
        leNumer->setText("");
        leRodzajObrotu->setText("Przyjęcie");
        dteDataObrotu->setDate(QDate::currentDate());
        cbKontrahent->setCurrentIndex(0);
    }else{
        query.prepare("SELECT [IdObrotu],[RodzajObrotu],[Numer],[IdKontrahenta],[DataObrotu] "
                      "FROM [Obroty_magazynowe] WHERE [IdObrotu] = " + QString::number(id));
        query.exec();

        while(query.next())
        {
            leRodzajObrotu->setText(query.value(1).toString());
            leNumer->setText(query.value(2).toString());
            cbKontrahent->setCurrentIndex(cbKontrahent->findData(query.value(3)));
            dteDataObrotu->setDate(query.value(4).toDate());
        }

        if (query.lastError() != QString())
        {
            QMessageBox::critical(this, tr("Błąd wczytywania"),
                                           query.lastError().text() + " ,\nquery: " + query.lastQuery(),
                                           QMessageBox::Ok);
        }
    }
    model->setFilter("[IdObrotu] = " + QString::number(id));
    model->select();
    if (model->lastError().isValid())
        qDebug() << "[ERROR] " << model->lastError().text();
    setWindowModified(false);
}

void Przyjecie::closeEvent(QCloseEvent *event)
{
    if (okToClearData())
    {
        writeSettings();
        emit sig_close();
        event->accept();
    }
    else
        event->ignore();
}

bool Przyjecie::okToClearData()
{
    if (isWindowModified())
    {
        QScopedPointer<QMessageBox> messageBox(new QMessageBox(this));
        messageBox->setWindowModality(Qt::WindowModal);
        messageBox->setIcon(QMessageBox::Question);
        messageBox->setWindowTitle(QString("%1 - %2")
                .arg(QApplication::applicationName()).arg("Niezapisane zmiany"));
        messageBox->setText("Czy zapisać zmiany?");
        if (!QString("").isEmpty())
            messageBox->setInformativeText("");
        messageBox->addButton(tr("Tak"), QMessageBox::YesRole);
        messageBox->addButton(tr("Nie"), QMessageBox::NoRole);
        messageBox->addButton(tr("Anuluj"), QMessageBox::RejectRole);
        messageBox->exec();
        if (messageBox->buttonRole(messageBox->clickedButton()) == QMessageBox::RejectRole)
            return false;
        if (messageBox->buttonRole(messageBox->clickedButton()) == QMessageBox::YesRole)
            return slot_Save();
        if (messageBox->buttonRole(messageBox->clickedButton()) == QMessageBox::NoRole)
            setWindowModified(false);
    }
    return true;
}

void Przyjecie::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);

    settings.beginGroup("Przyjecie");
    settings.setValue("column_width", view->horizontalHeader()->saveState());
    settings.setValue("max",isMaximized());
    if (!isMaximized())
    {
        settings.setValue("height",parentWidget()->height());
        settings.setValue("width",parentWidget()->width());
    }
    settings.endGroup();
}

void Przyjecie::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);

    settings.beginGroup("Przyjecie");
    view->horizontalHeader()->restoreState(settings.value("column_width",100).toByteArray());
    parentWidget()->resize(settings.value("width",300).toInt(),settings.value("height",300).toInt());
    settings.endGroup();
}

void Przyjecie::createWidgets()
{
    toolBar = new QToolBar();
    toolBar->setGeometry(0,0,200,20);
    actionNew = new QToolButton;
    actionNew->setGeometry(0,0,10,20);
    actionNew->setText(tr("Nowy"));
    actionSave = new QToolButton;
    actionSave->setText(tr("Zapisz"));
    actionDelete = new QToolButton;
    actionDelete->setText(tr("Usuń"));
    actionClose = new QToolButton;
    actionClose->setText(tr("Zakończ"));

    vblGlowny = new QVBoxLayout(this);

    lNumer = new QLabel(tr("Numer: "));
    lNumer->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lNumer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    leNumer = new QLineEdit();
    QSizePolicy sp = leNumer->sizePolicy();
    sp.setHorizontalStretch(1);
    leNumer->setSizePolicy(sp);

    lDataObrotu = new QLabel(tr("Data obrotu: "));
    lDataObrotu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lDataObrotu->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    dteDataObrotu = new QDateTimeEdit();
    dteDataObrotu->setCalendarPopup(true);
    dteDataObrotu->setDisplayFormat("yyyy-MM-dd");
    dteDataObrotu->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    lRodzajObrotu = new QLabel(tr("Rodzaj obrotu: "));
    lRodzajObrotu->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lRodzajObrotu->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    leRodzajObrotu = new QLineEdit();
    leRodzajObrotu->setDisabled(true);

    lKontrahent = new QLabel(tr("Kontrahent: "));
    lKontrahent->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lKontrahent->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    cbKontrahent = new QComboBox();
    query.prepare("SELECT IdKontrahenta,Nazwa FROM Kontrahenci ORDER BY Nazwa");
    query.exec();
    while(query.next()) {
        cbKontrahent->addItem(query.value(1).toString(),query.value(0));
    }

    gridLayout = new QGridLayout();
    hblDanePodstawowe = new QHBoxLayout();
    siDanePodstawowe = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    gbDanePodstawowe = new QGroupBox(tr("Dane podstawowe"));

    pbPozycjeAdd = new QPushButton("+");
    pbPozycjeAdd->setMaximumSize(24,24);
    pbPozycjeAdd->setMinimumSize(24,24);
    pbPozycjeDel = new QPushButton("-");
    pbPozycjeDel->setMaximumSize(24,24);
    pbPozycjeDel->setMinimumSize(24,24);
    siPozycje = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    hblPozycje = new QHBoxLayout();

    lPozycje = new QLabel(tr("Pozycje: "));

    model = new QSqlRelationalTableModel();
    model->setTable("Obroty_magazynowe_Pozycje");
    model->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
    //model->setSort(2,Qt::AscendingOrder);
    //model->setRelation(1, QSqlRelation("Obroty_magazynowe", "IdObrotu", "Numer"));
    model->setRelation(2, QSqlRelation("Produkty", "IdProduktu", "Nazwa"));
    //model->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    //model->setFilter("[IdObrotu] = " + QString::number(id));

    model->setHeaderData(0, Qt::Horizontal, tr("ID Pozycji"));
    model->setHeaderData(1, Qt::Horizontal, tr("ID Obrotu"));
    model->setHeaderData(2, Qt::Horizontal, tr("Produkty"));
    model->setHeaderData(3, Qt::Horizontal, tr("Ilość"));
    model->setHeaderData(4, Qt::Horizontal, tr("Cena"));

    model->select();
    if (model->lastError().isValid())
        qDebug() << "[ERROR] " << model->lastError().text();

    view = new QTableView();
    view->setModel(model);
    view->setSortingEnabled(true);
    view->resizeRowsToContents();
    view->setAlternatingRowColors(true);
    view->setItemDelegate(new QSqlRelationalDelegate(view));
    view->setColumnHidden(0, true);
    view->setColumnHidden(1, true);
    view->setSelectionBehavior(QAbstractItemView::SelectItems);
    view->setEditTriggers(QAbstractItemView::DoubleClicked);

    QHeaderView *verticalHeader = view->verticalHeader();
    verticalHeader->sectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);
}

void Przyjecie::createLayout()
{
    toolBar->addWidget(actionNew);
    toolBar->addWidget(actionSave);
    toolBar->addWidget(actionDelete);
    toolBar->addSeparator();
    toolBar->addWidget(actionClose);

    gridLayout->addWidget(lNumer, 0, 0, 1, 1);
    gridLayout->addWidget(leNumer, 0, 1, 1, 2);
    gridLayout->addWidget(lRodzajObrotu, 0, 3, 1, 1);
    gridLayout->addWidget(leRodzajObrotu, 0, 4, 1, 2);

    gridLayout->addWidget(lKontrahent, 1, 0, 1, 1);
    gridLayout->addWidget(cbKontrahent, 1, 1, 1, 2);
    gridLayout->addWidget(lDataObrotu, 1, 3, 1, 1);
    gridLayout->addWidget(dteDataObrotu, 1, 4, 1, 2);

    gbDanePodstawowe->setLayout(gridLayout);

    hblDanePodstawowe->addWidget(gbDanePodstawowe);
    hblDanePodstawowe->addSpacerItem(siDanePodstawowe);

    hblPozycje->addWidget(lPozycje);
    hblPozycje->addSpacerItem(siPozycje);
    hblPozycje->addWidget(pbPozycjeAdd);
    hblPozycje->addWidget(pbPozycjeDel);

    vblGlowny->addWidget(toolBar);
    vblGlowny->addLayout(hblDanePodstawowe);
    vblGlowny->addLayout(hblPozycje);
    vblGlowny->addWidget(view);
}

void Przyjecie::createConnections()
{
    connect(actionNew, SIGNAL(clicked()),this, SLOT(slot_New()));
    connect(actionClose, SIGNAL(clicked()),this, SLOT(slot_Close()));
    connect(actionDelete, SIGNAL(clicked()),this, SLOT(slot_Delete()));
    connect(actionSave, SIGNAL(clicked()),this, SLOT(slot_Save()));
    connect(leNumer,SIGNAL(textChanged(QString)),this,SLOT(setDirty()));
    connect(leNumer,SIGNAL(textChanged(QString)),this,SLOT(setTitle()));
    connect(dteDataObrotu,SIGNAL(editingFinished()),this,SLOT(setDirty()));
    connect(cbKontrahent,SIGNAL(currentIndexChanged(int)),this,SLOT(setDirty()));

    connect(pbPozycjeAdd, SIGNAL(clicked()),this, SLOT(slot_PozycjeAdd()));
    connect(pbPozycjeDel, SIGNAL(clicked()),this, SLOT(slot_PozycjeDel()));
}

void Przyjecie::slot_New()
{
    id = 0;
    loadData();
}

void Przyjecie::slot_Close()
{
    parentWidget()->close();
}

void Przyjecie::slot_Delete()
{
    if (id > 0)
    {
        switch(QMessageBox::question(
                        this,
                        tr("Usuń"),
                        tr("Czy potwierdzasz usunięcie?"),
                        QMessageBox::Yes |
                        QMessageBox::No,
                        QMessageBox::No ) )
        {
            case QMessageBox::Yes:
            {
                query.prepare("DELETE FROM [Obroty_magazynowe] WHERE [IdObrotu] = " + QString::number(id));
                query.exec();
                if(query.lastError() != QString())
                {
                    QMessageBox::critical(this, tr("Błąd usuwania"),
                                                   query.lastError().text(),
                                                   QMessageBox::Ok);
                    return;
                }
            }
                break;
            case QMessageBox::No:
                return;
                break;
            default:
                return;
                break;
        }

    }
    parentWidget()->close();
}

bool Przyjecie::slot_Save()
{
    if (!checkRequiredFields())
        return false;
    QSqlDatabase::database().transaction();
    if (id == 0)
    {
        query.prepare("INSERT INTO [Obroty_magazynowe] ([RodzajObrotu],[Numer],[IdKontrahenta],[DataObrotu]) "
                      "VALUES (:Rodzaj,:Numer,:IdKontrahenta,:DataObrotu)");
        query.bindValue(":Rodzaj", leRodzajObrotu->text());
        query.bindValue(":Numer", leNumer->text());
        query.bindValue(":IdKontrahenta", cbKontrahent->itemData(cbKontrahent->currentIndex()));
        query.bindValue(":DataObrotu", dteDataObrotu->date().toString("yyyy-MM-dd"));
        if (!query.exec())
        {
            QMessageBox::critical(this, tr("Błąd zapisu"),
                                           query.lastError().text() + " ,\nquery: " + query.lastQuery(),
                                           QMessageBox::Ok);
            return false;
        }
        id = query.lastInsertId().toInt();
        int row = model->rowCount();
        for(int i = 0; i < row; i++)
        {
            QModelIndex idx = model->index(i, 1);
            model->setData(idx, id);
        }
        model->submitAll();
    }else{
        query.prepare("UPDATE [Obroty_magazynowe] SET [Numer] = :Numer, [IdKontrahenta] = :IdKontrahenta, [DataObrotu] = :DataObrotu"
                      " WHERE [IdObrotu] = :IdObrotu");
        query.bindValue(":IdObrotu", id);
        query.bindValue(":Numer", leNumer->text());
        query.bindValue(":DataObrotu", dteDataObrotu->date().toString("yyyy-MM-dd"));
        query.bindValue(":IdKontrahenta", cbKontrahent->itemData(cbKontrahent->currentIndex()));
        if (!query.exec())
        {
            QMessageBox::critical(this, tr("Błąd zapisu"),
                                           query.lastError().text() + " ,\nquery: " + query.lastQuery(),
                                           QMessageBox::Ok);
            return false;
        }
        model->submitAll();
    }
    QSqlDatabase::database().commit();
    setWindowModified(false);
    loadData();
    return true;
}

void Przyjecie::slot_PozycjeAdd()
{
    int row = model->rowCount();
    model->insertRow(row);
    view->selectRow(row);
    QModelIndex idx = model->index(row, 1);
    model->setData(idx, id);
}

void Przyjecie::slot_PozycjeDel()
{
    model->removeRow(view->currentIndex().row());
}

bool Przyjecie::checkRequiredFields()
{
    bool ret = true;
    if (leNumer->text().isEmpty())
    {
        leNumer->setStyleSheet("border: 1px solid red");
        ret = false;
    }else{
        leNumer->setStyleSheet("");
    }
    return ret;
}
