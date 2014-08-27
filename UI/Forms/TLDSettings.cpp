#include "TLDSettings.h"
#include "ui_TLDSettings.h"

TLDSettings::TLDSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TLDSettings),
    currentAlgo(0)
{
    if(ui)
    {
        ui->setupUi(this);

        connect(ui->exportModelCheckBox, SIGNAL(clicked()), this, SLOT(UpdateSettingsSlot()));
        connect(ui->showTrajectoryCheckBox, SIGNAL(clicked()), this, SLOT(UpdateSettingsSlot()));
        connect(ui->loadModelCheckBox, SIGNAL(clicked()), this, SLOT(UpdateSettingsSlot()));
        connect(ui->threshDoubleSpinBox, SIGNAL(valueChanged(double)), SLOT(UpdateThreshold(double)));
        connect(ui->trajLegnthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateTrajectoryLength(int)));
    }
}

void TLDSettings::Register(AlgoVision *algo)
{
    currentAlgo = 0;

    currentAlgo = static_cast<SentryTLD*>(algo);
}

void TLDSettings::DisplaySettings()
{
    if(currentAlgo)
    {
        ui->exportModelCheckBox->setChecked(currentAlgo->Settings.exportModelAfterRun);
        ui->showTrajectoryCheckBox->setChecked(currentAlgo->Settings.showTrajectory);
        ui->loadModelCheckBox->setChecked(currentAlgo->Settings.loadModel);
        ui->threshDoubleSpinBox->setValue(currentAlgo->Settings.threshold);
        ui->trajLegnthSpinBox->setValue(currentAlgo->Settings.trajectoryLength);
    }
}

void TLDSettings::UpdateSettings()
{
    if(currentAlgo)
    {
        currentAlgo->Settings.exportModelAfterRun = ui->exportModelCheckBox->isChecked();
        currentAlgo->Settings.showTrajectory      = ui->showTrajectoryCheckBox->isChecked();
        currentAlgo->Settings.loadModel           = ui->loadModelCheckBox->isChecked();
    }
}

void TLDSettings::UpdateSettingsSlot()
{
    UpdateSettings();
}

void TLDSettings::UpdateThreshold(double val)
{
    if(currentAlgo && val >= 0 && val <= 1)
    {
        currentAlgo->Settings.threshold = val;
    }
}

void TLDSettings::UpdateTrajectoryLength(int val)
{
    if(currentAlgo && val >= 0 && val <= 100)
    {
        currentAlgo->Settings.trajectoryLength = val;
    }
}

TLDSettings::~TLDSettings()
{
    delete ui;
}
