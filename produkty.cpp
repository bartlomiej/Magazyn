#include "produkty.h"

Produkty::Produkty(QWidget *parent) :
    QWidget(parent)
{
    stan = Display;
    setAttribute(Qt::WA_DeleteOnClose);
    createWidgets();
    createLayout();
    createConnections();
    readSettings();
}

Produkty::~Produkty()
{
}

void Produkty::closeEvent(QCloseEvent *event)
{
    if (stan == Display)
    {
        writeSettings();
        emit sig_close();
        event->accept();
    }
    else
    {
        QScopedPointer<QMessageBox> messageBox(new QMessageBox(this));
        messageBox->setWindowModality(Qt::WindowModal);
        messageBox->setIcon(QMessageBox::Question);
        messageBox->setWindowTitle(QString("%1 - %2")
                .arg(QApplication::applicationName()).arg("Niezapisane zmiany"));
        messageBox->setText("Czy zapisać zmiany?");
        if (!QString("").isEmpty())
            messageBox->setInformativeText("");
        messageBox->addButton(QMessageBox::Save);
        messageBox->addButton(QMessageBox::Discard);
        messageBox->addButton(QMessageBox::Cancel);
        messageBox->setDefaultButton(QMessageBox::Save);
        messageBox->exec();
        if (messageBox->clickedButton() == messageBox->button(QMessageBox::Cancel))
        {
            event->ignore();
        }
        if (messageBox->clickedButton() == messageBox->button(QMessageBox::Save))
        {
            if (slot_Save())
            {
                writeSettings();
                event->accept();
            }else{
                event->ignore();
            }

        }
    }
}

void Produkty::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);
    settings.beginGroup("Produkty");
    parentWidget()->resize(settings.value("width",300).toInt(),settings.value("height",300).toInt());
    view->horizontalHeader()->restoreState(settings.value("column_width",100).toByteArray());
    settings.endGroup();
}

void Produkty::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);
    settings.beginGroup("Produkty");
    settings.setValue("height",parentWidget()->height());
    settings.setValue("width",parentWidget()->width());
    settings.setValue("column_width", view->horizontalHeader()->saveState());
    settings.endGroup();
}


void Produkty::createWidgets()
{
    toolBar = new QToolBar();
    actionNew = new QAction(tr("&Nowy"), this);
    actionNew->setShortcut(QKeySequence::New);
    actionEdit = new QAction(tr("&Edytuj"), this);
    actionEdit->setShortcut(Qt::CTRL + Qt::Key_E);
    actionSave = new QAction(tr("&Zapisz"), this);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    actionCancel = new QAction(tr("&Cancel"), this);
    actionCancel->setEnabled(false);
    actionDelete = new QAction(tr("&Usuń"), this);
    actionDelete->setShortcut(QKeySequence::Delete);
    actionClose = new QAction(tr("&Zamknij"), this);
    actionClose->setShortcut(QKeySequence::Close);

    model.setTable("Produkty");
    model.setSort(1,Qt::AscendingOrder);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setHeaderData(0, Qt::Horizontal, QObject::tr("ID Produktu"));
    model.setHeaderData(1, Qt::Horizontal, QObject::tr("Indeks"));
    model.setHeaderData(2, Qt::Horizontal, QObject::tr("Nazwa"));
    if (!model.select())
    {
        qDebug() << "[ERROR] " << model.lastError().text();
    }

    view = new QTableView();
    view->setModel(&model);
    view->setColumnHidden(0, true);
    view->resizeRowsToContents();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setAlternatingRowColors(true);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Produkty::createLayout()
{
    toolBar->addAction(actionNew);
    toolBar->addAction(actionEdit);
    toolBar->addAction(actionDelete);
    toolBar->addSeparator();
    toolBar->addAction(actionSave);
    toolBar->addAction(actionCancel);
    toolBar->addSeparator();
    toolBar->addAction(actionClose);
    gridLayout = new QGridLayout();
    lIndeks = new QLabel(tr("Indeks: "));
    lIndeks->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leIndeks = new QLineEdit();
    lNazwa = new QLabel(tr("Nazwa: "));
    lNazwa->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leNazwa = new QLineEdit();
    gbEdit = new QGroupBox();
    gridLayout->addWidget(lIndeks, 0, 0, 1, 1);
    gridLayout->addWidget(leIndeks, 0, 1, 1, 1);
    gridLayout->addWidget(lNazwa, 1, 0, 1, 1);
    gridLayout->addWidget(leNazwa, 1, 1, 1, 1);
    gbEdit->setLayout(gridLayout);
    gbEdit->setEnabled(false);

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(&model);
    mapper->setItemDelegate(new ProduktyDelegate());
    mapper->addMapping(leIndeks, 1);
    mapper->addMapping(leNazwa, 2);
    mapper->toFirst();

    centralLayout = new QVBoxLayout();
    centralLayout->setContentsMargins(2,2,2,2);
    centralLayout->setSpacing(2);
    centralLayout->addWidget(toolBar);
    centralLayout->addWidget(gbEdit);
    centralLayout->addWidget(view);
    setLayout(centralLayout);
}

void Produkty::createConnections()
{
    connect(view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(actionEdit, SIGNAL(triggered()), this, SLOT(slot_Edit()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(slot_Save()));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(slot_New()));
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(slot_Delete()));
    connect(actionCancel, SIGNAL(triggered()), this, SLOT(slot_Cancel()));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(slot_Close()));
    connect(&model,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(setDirty()));
    connect(&model,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(setDirty()));
    connect(&model,SIGNAL(modelReset()),this,SLOT(setDirty()));
}

void Produkty::slot_New()
{
    stan = New;
    int row = model.rowCount();
    model.insertRow(row);
    view->selectRow(row);
    actionNew->setEnabled(false);
    actionEdit->setEnabled(false);
    actionDelete->setEnabled(false);
    actionClose->setEnabled(false);
    actionSave->setEnabled(true);
    actionCancel->setEnabled(true);
    gbEdit->setEnabled(true);
    view->setEnabled(false);
}

void Produkty::slot_Edit()
{
    stan = Edit;
    actionNew->setEnabled(false);
    actionEdit->setEnabled(false);
    actionDelete->setEnabled(false);
    actionClose->setEnabled(false);
    actionSave->setEnabled(true);
    actionCancel->setEnabled(true);
    gbEdit->setEnabled(true);
    view->setEnabled(false);
}

void Produkty::slot_Delete()
{
    stan = Delete;
    actionNew->setEnabled(false);
    actionEdit->setEnabled(false);
    actionDelete->setEnabled(false);
    actionClose->setEnabled(false);
    actionSave->setEnabled(true);
    actionCancel->setEnabled(true);
    model.removeRow(view->currentIndex().row());
}

void Produkty::slot_Cancel()
{
    model.revertAll();
    leIndeks->setStyleSheet("");
    leNazwa->setStyleSheet("");
    actionNew->setEnabled(true);
    actionEdit->setEnabled(true);
    actionDelete->setEnabled(true);
    actionClose->setEnabled(true);
    actionSave->setEnabled(false);
    actionCancel->setEnabled(false);
    gbEdit->setEnabled(false);
    view->setEnabled(true);
    stan = Display;
}

void Produkty::slot_Close()
{
    parentWidget()->close();
}

bool Produkty::slot_Save()
{
    stan = Save;
    mapper->submit();
    model.submitAll();
    if(model.lastError().isValid())
    {
        QMessageBox::critical(this, tr("Błąd zapisu"),
                                       model.lastError().text(),
                                       QMessageBox::Ok);
        stan = Edit;
        return 0;
    }
    actionNew->setEnabled(true);
    actionEdit->setEnabled(true);
    actionDelete->setEnabled(true);
    actionClose->setEnabled(true);
    actionSave->setEnabled(false);
    actionCancel->setEnabled(false);
    gbEdit->setEnabled(false);
    view->setEnabled(true);
    stan = Display;
    return 1;
}

ProduktyDelegate::ProduktyDelegate(QObject *parent)
    :QSqlRelationalDelegate(parent)
{
}

void ProduktyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    editor->setProperty("text", index.data());
}

void ProduktyDelegate::setModelData(QWidget *editor,
                         QAbstractItemModel *model,
                         const QModelIndex &index) const
{
    model->setData(index, editor->property("text") == "" ?
                       QVariant() :
                       editor->property("text"));
    if (model->data(index) == QVariant())
    {
        editor->setStyleSheet("border: 1px solid red");
    }else{
        editor->setStyleSheet("");
    }
}
