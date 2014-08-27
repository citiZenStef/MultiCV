#include "listefiltres.h"
#include "ui_listefiltres.h"

ListeFiltres::ListeFiltres(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListeFiltres)
{
    ui->setupUi(this);
}

QString ListeFiltres::filtreChoisi() const
{
    QListWidgetItem* currentItem = ui->FiltresListWidget->currentItem();
    if(currentItem)
    {
        return currentItem->text();
    }
    return QString();
}

ListeFiltres::~ListeFiltres()
{
    delete ui;
}
