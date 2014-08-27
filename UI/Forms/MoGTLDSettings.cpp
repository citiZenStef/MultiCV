#include "MoGTLDSettings.h"
#include "ui_MoGTLDSettings.h"

MoGTLDSettings::MoGTLDSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoGTLDSettings),
    currentAlgo(0)
{
    if(ui)
    {
        ui->setupUi(this);

        connect(ui->exportModelCheckBox,       SIGNAL(clicked()), this, SLOT(UpdateSettingsSlot()));
        connect(ui->showTrajectoryCheckBox,    SIGNAL(clicked()), this, SLOT(UpdateSettingsSlot()));
        connect(ui->loadModelCheckBox,         SIGNAL(clicked()), this, SLOT(UpdateSettingsSlot()));
        connect(ui->threshDoubleSpinBox,       SIGNAL(valueChanged(double)), SLOT(UpdateThreshold(double)));
        connect(ui->learningRateDoubleSpinBox, SIGNAL(valueChanged(double)), SLOT(UpdateLearningRate(double)));
        connect(ui->trajLegnthSpinBox,         SIGNAL(valueChanged(int)), this, SLOT(UpdateTrajectoryLength(int)));
    }
}

void MoGTLDSettings::Register(AlgoVision *algo)
{
    currentAlgo = 0;

    currentAlgo = static_cast<MoGTLD*>(algo);
}

void MoGTLDSettings::DisplaySettings()
{
    if(currentAlgo)
    {
        ui->exportModelCheckBox      ->setChecked(currentAlgo->TLDSettings.exportModelAfterRun);
        ui->showTrajectoryCheckBox   ->setChecked(currentAlgo->TLDSettings.showTrajectory);
        ui->loadModelCheckBox        ->setChecked(currentAlgo->TLDSettings.loadModel);
        ui->threshDoubleSpinBox      ->setValue  (currentAlgo->TLDSettings.threshold);
        ui->learningRateDoubleSpinBox->setValue  (currentAlgo->MOGSettings.LearningRate);
        ui->trajLegnthSpinBox        ->setValue  (currentAlgo->TLDSettings.trajectoryLength);
    }
}

void MoGTLDSettings::UpdateSettings()
{
    if(currentAlgo)
    {
        currentAlgo->TLDSettings.exportModelAfterRun = ui->exportModelCheckBox->isChecked();
        currentAlgo->TLDSettings.showTrajectory      = ui->showTrajectoryCheckBox->isChecked();
        currentAlgo->TLDSettings.loadModel           = ui->loadModelCheckBox->isChecked();
    }
}

void MoGTLDSettings::UpdateSettingsSlot()
{
    UpdateSettings();
}

void MoGTLDSettings::UpdateThreshold(double val)
{
    if(currentAlgo && val >= 0 && val <= 1)
    {
        currentAlgo->TLDSettings.threshold = val;
    }
}

void MoGTLDSettings::UpdateLearningRate(double val)
{
    if(currentAlgo && val >= 0 && val <= 1)
    {
        currentAlgo->MOGSettings.LearningRate = val;
    }
}

void MoGTLDSettings::UpdateTrajectoryLength(int val)
{
    if(currentAlgo && val >= 0 && val <= 100)
    {
        currentAlgo->TLDSettings.trajectoryLength = val;
    }
}

MoGTLDSettings::~MoGTLDSettings()
{
    delete ui;
}
