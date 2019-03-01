#include "przyjecia.h"

Przyjecia::Przyjecia(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    createWidgets();
    createLayout();
    createConnections();
    readSettings();
}

Przyjecia::~Przyjecia()
{
}

void Przyjecia::closeEvent(QCloseEvent *event)
{
    writeSettings();
    emit sig_close();
    event->accept();
}

void Przyjecia::writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);

    settings.beginGroup("Przyjecia");
    settings.setValue("column_width", view->horizontalHeader()->saveState());
    settings.setValue("max",isMaximized());
    if (!isMaximized())
    {
        settings.setValue("height",parentWidget()->height());
        settings.setValue("width",parentWidget()->width());
    }
    settings.endGroup();
}

void Przyjecia::readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.setFallbacksEnabled(false);

    settings.beginGroup("Przyjecia");
    view->horizontalHeader()->restoreState(settings.value("column_width",100).toByteArray());
    parentWidget()->resize(settings.value("width",300).toInt(),settings.value("height",300).toInt());
    settings.endGroup();
}

void Przyjecia::createWidgets()
{
    toolBar = new QToolBar();
    toolBar->setGeometry(0,0,200,20);
    actionNew = new QToolButton;
    actionNew->setGeometry(0,0,10,20);
    actionNew->setText(tr("Nowy"));
    actionEdit = new QToolButton;
    actionEdit->setText(tr("Edytuj"));
    actionDelete = new QToolButton;
    actionDelete->setText(tr("Usuń"));
    actionRefresh = new QToolButton;
    actionRefresh->setText(tr("Odśwież"));
    actionClose = new QToolButton;
    actionClose->setText(tr("Zakończ"));

    vblGlowny = new QVBoxLayout(this);

    model = new QSqlRelationalTableModel(this);
    model->setTable("Obroty_magazynowe");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setSort(2,Qt::AscendingOrder);
    model->setRelation(3, QSqlRelation("Kontrahenci", "IdKontrahenta", "Nazwa"));
    model->setJoinMode(QSqlRelationalTableModel::InnerJoin);
    model->setFilter("RodzajObrotu = 'Przyjęcie'");
    model->select();

    if (model->lastError().isValid())
        qDebug() << model->lastError().text();

    model->setHeaderData(0, Qt::Horizontal, tr("ID Obrotu"));
    model->setHeaderData(1, Qt::Horizontal, tr("Rodzaj obrotu"));
    model->setHeaderData(2, Qt::Horizontal, tr("Numer"));
    model->setHeaderData(3, Qt::Horizontal, tr("Kontrahent"));
    model->setHeaderData(4, Qt::Horizontal, tr("Data obrotu"));

    proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);

    view = new QTableView();
    view->setModel(proxy);
    view->setSortingEnabled(true);
    view->resizeRowsToContents();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setAlternatingRowColors(true);
    view->setColumnHidden(0, true);
    view->setColumnHidden(1, true);

    QHeaderView *verticalHeader = view->verticalHeader();
    verticalHeader->sectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);
}

void Przyjecia::createLayout()
{
    toolBar->addWidget(actionNew);
    toolBar->addWidget(actionEdit);
    toolBar->addWidget(actionDelete);
    toolBar->addSeparator();
    toolBar->addWidget(actionRefresh);
    toolBar->addSeparator();
    toolBar->addWidget(actionClose);

    vblGlowny->addWidget(toolBar);
    vblGlowny->addWidget(view);
}

void Przyjecia::createConnections()
{
    connect(actionNew, SIGNAL(clicked()),this, SLOT(slot_New()));
    connect(actionEdit, SIGNAL(clicked()),this, SLOT(slot_Edit()));
    connect(actionRefresh, SIGNAL(clicked()),this, SLOT(slot_Refresh()));
    connect(actionClose, SIGNAL(clicked()),this, SLOT(slot_Close()));
    connect(actionDelete, SIGNAL(clicked()),this, SLOT(slot_Delete()));
    connect(view, SIGNAL(doubleClicked(const QModelIndex &)),this, SLOT(slot_view_doubleClicked(const QModelIndex &)));
}

void Przyjecia::slot_Close()
{
    parentWidget()->close();
}

void Przyjecia::slot_view_doubleClicked(const QModelIndex &index)
{
    int id = proxy->index(index.row(),0).data().toInt();
    emit sig_open(id);
}

void Przyjecia::slot_New()
{
    int id = 0;
    emit sig_open(id);
}

void Przyjecia::slot_Edit()
{
    int id = proxy->index(view->currentIndex().row(),0).data().toInt();
    if (id == 0)
    {
        view->setCurrentIndex(proxy->index(0,0));
        id = proxy->index(view->currentIndex().row(),0).data().toInt();
    }
    emit sig_open(id);
}

void Przyjecia::slot_Delete()
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
            proxy->removeRow(view->currentIndex().row());
            if (!model->submitAll())
            {
                QMessageBox::critical(this, tr("Błąd"),
                                      model->lastError().text(),
                                      QMessageBox::Ok);
            }
        }
            break;
        case QMessageBox::No:
            qDebug( "no" );
            break;
        default:
            qDebug( "close" );
            break;
    }
}

void Przyjecia::slot_Refresh()
{
    model->select();
}
