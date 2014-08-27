#ifndef MOGSETTINGS_H
#define MOGSETTINGS_H

#include <QWidget>
#include "AlgoSettings.h"
#include "Vision/SentryMoG.h"

namespace Ui {
class MOGSettings;
}

class MOGSettings : public QWidget, public AlgoSettings
{
    Q_OBJECT
    
public:
    explicit MOGSettings(QWidget *parent = 0);

    void Register(AlgoVision* algo);
    void DisplaySettings();
    void UpdateSettings();

    ~MOGSettings();
    
private:
    Ui::MOGSettings *ui;
};

#endif // MOGSETTINGS_H
