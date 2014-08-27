#ifndef LISTEFILTRES_H
#define LISTEFILTRES_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class ListeFiltres;
}

class ListeFiltres : public QDialog
{
    Q_OBJECT
    
public:
    explicit ListeFiltres(QWidget *parent = 0);
    QString filtreChoisi() const;
    ~ListeFiltres();
    
private:
    Ui::ListeFiltres *ui;
};

#endif // LISTEFILTRES_H
