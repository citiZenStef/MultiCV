#include "MOGSettings.h"
#include "ui_MOGSettings.h"

MOGSettings::MOGSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MOGSettings)
{
    ui->setupUi(this);
}

void MOGSettings::Register(AlgoVision *algo)
{
}

void MOGSettings::DisplaySettings()
{
}

void MOGSettings::UpdateSettings()
{

}

MOGSettings::~MOGSettings()
{
    delete ui;
}
