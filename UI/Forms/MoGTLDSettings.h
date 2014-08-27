#ifndef MoGTLDSettings_H
#define MoGTLDSettings_H

#include <QWidget>
#include "AlgoSettings.h"
#include "Vision/MoGTLD.h"

namespace Ui {
class MoGTLDSettings;
}

class MoGTLDSettings : public QWidget, public AlgoSettings
{
    Q_OBJECT
    
public:
    explicit MoGTLDSettings(QWidget *parent = 0);

    void Register(AlgoVision* algo);
    void DisplaySettings();
    void UpdateSettings();

    ~MoGTLDSettings();
    
private:
    Ui::MoGTLDSettings *ui;
    MoGTLD* currentAlgo;

private slots:
    void UpdateSettingsSlot();
    void UpdateThreshold(double val);
    void UpdateLearningRate(double val);
    void UpdateTrajectoryLength(int val);
};

#endif // MoGTLDSettings_H
