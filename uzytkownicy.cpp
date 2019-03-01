#include "uzytkownicy.h"

Uzytkownicy::Uzytkownicy(QWidget *parent) :
    QWidget(parent)
{
    stan = Display;
    setAttribute(Qt::WA_DeleteOnClose);
    createWidgets();
    createLayout();
    createConnections();
    readSettings();
}

Uzytkownicy::~Uzytkownicy()
{

}

void Uzytkownicy::closeEvent(QCloseEvent *event)
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

void Uzytkownicy::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);
    settings.beginGroup("Uzytkownicy");
    parentWidget()->resize(settings.value("width",300).toInt(),settings.value("height",300).toInt());
    view->horizontalHeader()->restoreState(settings.value("column_width",100).toByteArray());
    settings.endGroup();
}

void Uzytkownicy::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);
    settings.beginGroup("Uzytkownicy");
    settings.setValue("height",parentWidget()->height());
    settings.setValue("width",parentWidget()->width());
    settings.setValue("column_width", view->horizontalHeader()->saveState());
    settings.endGroup();
}


void Uzytkownicy::createWidgets()
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

    model.setTable("Uzytkownicy");
    model.setSort(1,Qt::AscendingOrder);
    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
    model.setHeaderData(0, Qt::Horizontal, QObject::tr("ID Użytkownika"));
    model.setHeaderData(1, Qt::Horizontal, QObject::tr("Nazwisko"));
    model.setHeaderData(2, Qt::Horizontal, QObject::tr("Imię"));
    model.setHeaderData(3, Qt::Horizontal, QObject::tr("Login"));
    model.setHeaderData(4, Qt::Horizontal, QObject::tr("Hasło"));
    if (!model.select())
    {
        qDebug() << "[ERROR] " << model.lastError().text();
    }

    view = new QTableView();
    view->setModel(&model);
    view->setColumnHidden(0, true);
    view->setColumnHidden(4, true);
    view->resizeRowsToContents();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setAlternatingRowColors(true);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Uzytkownicy::createLayout()
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
    lNazwisko = new QLabel(tr("Nazwisko: "));
    lNazwisko->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leNazwisko = new QLineEdit();
    lImie = new QLabel(tr("Imię: "));
    lImie->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leImie = new QLineEdit();
    lLogin = new QLabel(tr("Login: "));
    lLogin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leLogin = new QLineEdit();
    lHaslo = new QLabel(tr("Hasło: "));
    lHaslo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    leHaslo = new QLineEdit();
    leHaslo->setEchoMode(QLineEdit::Password);
    gbEdit = new QGroupBox();
    gridLayout->addWidget(lNazwisko, 0, 0, 1, 1);
    gridLayout->addWidget(leNazwisko, 0, 1, 1, 1);
    gridLayout->addWidget(lImie, 1, 0, 1, 1);
    gridLayout->addWidget(leImie, 1, 1, 1, 1);
    gridLayout->addWidget(lLogin, 2, 0, 1, 1);
    gridLayout->addWidget(leLogin, 2, 1, 1, 1);
    gridLayout->addWidget(lHaslo, 3, 0, 1, 1);
    gridLayout->addWidget(leHaslo, 3, 1, 1, 1);
    gbEdit->setLayout(gridLayout);
    gbEdit->setEnabled(false);

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(&model);
    mapper->setItemDelegate(new UzytkownicyDelegate());
    mapper->addMapping(leNazwisko, 1);
    mapper->addMapping(leImie, 2);
    mapper->addMapping(leLogin, 3);
    mapper->addMapping(leHaslo, 4);
    mapper->toFirst();

    centralLayout = new QVBoxLayout();
    centralLayout->setContentsMargins(2,2,2,2);
    centralLayout->setSpacing(2);
    centralLayout->addWidget(toolBar);
    centralLayout->addWidget(gbEdit);
    centralLayout->addWidget(view);
    setLayout(centralLayout);
}

void Uzytkownicy::createConnections()
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

void Uzytkownicy::slot_New()
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

void Uzytkownicy::slot_Edit()
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

void Uzytkownicy::slot_Delete()
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

void Uzytkownicy::slot_Cancel()
{
    model.revertAll();
    leNazwisko->setStyleSheet("");
    leImie->setStyleSheet("");
    leLogin->setStyleSheet("");
    leHaslo->setStyleSheet("");
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

void Uzytkownicy::slot_Close()
{
    parentWidget()->close();
}

bool Uzytkownicy::slot_Save()
{
    stan = Save;
    QByteArray result = hash->hash(leHaslo->text().toLatin1(),QCryptographicHash::Md5);
    QString hashResult(result.toHex());
    int row = view->currentIndex().row();
    view->model()->setData(view->model()->index(row,4),hashResult);
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

UzytkownicyDelegate::UzytkownicyDelegate(QObject *parent)
    :QSqlRelationalDelegate(parent)
{
}

void UzytkownicyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    editor->setProperty("text", index.data());
}

void UzytkownicyDelegate::setModelData(QWidget *editor,
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
