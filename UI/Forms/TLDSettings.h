#ifndef TLDSETTINGS_H
#define TLDSETTINGS_H

#include <QWidget>
#include "Camera/Structures.h"
#include "AlgoSettings.h"
#include "Vision/SentryTLD.h"

namespace Ui {
class TLDSettings;
}

class TLDSettings : public QWidget, public AlgoSettings
{
    Q_OBJECT
    
public:
    explicit TLDSettings(QWidget *parent = 0);

    void Register(AlgoVision *algo);
    void DisplaySettings();
    void UpdateSettings();

    ~TLDSettings();
    
private:
    Ui::TLDSettings *ui;
    SentryTLD* currentAlgo;

private slots:
    void UpdateSettingsSlot();
    void UpdateThreshold(double val);
    void UpdateTrajectoryLength(int val);
};

#endif // TLDSETTINGS_H
