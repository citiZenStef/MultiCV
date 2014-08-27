/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* MainWindow.cpp                                                       */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/* Adaptation from:                                                     */
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/*                                                                      */
/* Copyright (c) 2012-2013 Nick D'Ademo                                 */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CalibrationDialog.h"
#include "Camera/Structures.h"

#include <QtCore/QThread>


//////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    IsCameraOpened( false ),
    IsVideoOpened( false ),
    IpCamCounter( 0 )
{
    ui->setupUi(this);

    // Create SharedImageBuffer object
    SImageBuffer = new SharedImageBuffer();

    AIStateImg.load( ":/red" );
    LogoImg.   load( ":/cible" );

    ui->tabWidget->addTab(new VisionUnit_widget(ui->tabWidget), "[ ]");

    TLDAlgoSettings    = new TLDSettings();
    MOGAlgoSettings    = new MOGSettings();
    MOGTLDAlgoSettings = new MoGTLDSettings();

    StackedWidget = new QStackedWidget;

    StackedWidget->addWidget(TLDAlgoSettings);
    StackedWidget->addWidget(MOGAlgoSettings);
    StackedWidget->addWidget(MOGTLDAlgoSettings);

    ui->visionSettingsVerticalLayout->addWidget(StackedWidget);

    connect( ui->startStreamPushButton, SIGNAL( clicked() ),   this, SLOT( InitVisionUnit() ) );
    connect( ui->closeStreamPushButton, SIGNAL( clicked() ),   this, SLOT( CloseByTab() ) );
    connect( ui->actionCalibration,     SIGNAL( triggered() ), this, SLOT( Calib() ) );
    connect( ui->algoComboBox,          SIGNAL( activated(int) ), StackedWidget, SLOT( setCurrentIndex(int) ) );

    // Register type
    qRegisterMetaType<struct ThreadStatisticsData>("ThreadStatisticsData");
}


//////////////////////////////////////////////////////////////////

void MainWindow::Close()
{
}


//////////////////////////////////////////////////////////////////

void MainWindow::CloseByTab()
{
    int tabIdx = ui->tabWidget->currentIndex();

    int deviceNumber = DeviceNumbersMap.key( tabIdx );

    if(VisionUnitsMap.size() > 0 && VisionUnitsMap[ deviceNumber ])
    {
        VisionUnitsMap[ deviceNumber ]->Terminate();

        // remove from map by tab index
        removeFromMapByTabIndex( DeviceNumbersMap, tabIdx );
        removeFromMapByTabIndex( VisionUnitWidgetsMap, deviceNumber );
        removeFromMapByTabIndex( VisionUnitsMap, deviceNumber );

        ui->tabWidget->removeTab( tabIdx );

        updateMapValues(DeviceNumbersMap, tabIdx);

        if(ui->tabWidget->count() == 0)
            ui->tabWidget->addTab(new VisionUnit_widget(ui->tabWidget), "[ ]");
    }
}


//////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    delete ui;
}


//////////////////////////////////////////////////////////////////

void MainWindow::InitVisionUnit()
{
  CameraConnectDialog *cameraConnectDialog = new CameraConnectDialog( this );

  if( cameraConnectDialog->exec() == QDialog::Accepted )
  {
      int deviceNumber = cameraConnectDialog->getDeviceNumber();
      int nextTabIndex = ui->tabWidget->count();

      QString fileName;
      QString URL;

      if(cameraConnectDialog->isVideoUsed())
      {
          // Get file name
          fileName = QFileDialog::getOpenFileName( this,
                                                   tr("Open video file"),
                                                   "",
                                                   tr("*.*") );

          // Add a vision unit UI
          if(!VisionUnitWidgetsMap[ deviceNumber ])
              VisionUnitWidgetsMap[ deviceNumber ] = new VisionUnit_widget(ui->tabWidget, fileName);


          QString tabLabel = cameraConnectDialog->getTabLabel();

          if(!IsVideoOpened)
          {
              if(DeviceNumbersMap.size() == 0)
              {
                  DeviceNumbersMap    [ deviceNumber ] = 0;

                  ui->tabWidget->removeTab(0);
                  ui->tabWidget->insertTab(0, VisionUnitWidgetsMap[ deviceNumber ], tabLabel + " [" + QString::number(deviceNumber) + "]");

                  IsVideoOpened = true;
              }
              else
              {
                  DeviceNumbersMap    [ deviceNumber ] = nextTabIndex;

                  ui->tabWidget->addTab(VisionUnitWidgetsMap[ deviceNumber ], tabLabel + " [" + QString::number(deviceNumber) + "]");

                  IsVideoOpened = true;
              }

              // Set the Vision Unit's current vision algorithm
              AlgoVision* algo = VisionUnitWidgetsMap[ deviceNumber ]->SetVisionAlgorithm( ui->algoComboBox->currentText() );
              if(algo)
              {
                  // Connect the algo to its corresponding UI
                  int idx = ui->algoComboBox->currentIndex();

                  switch(idx) {
                  case 0 :
                      TLDAlgoSettings->Register(algo);
                      TLDAlgoSettings->DisplaySettings();
                      break;
                  case 1 :
                      MOGAlgoSettings->Register(algo);
                      MOGAlgoSettings->DisplaySettings();
                      break;
                  case 2 :
                      MOGTLDAlgoSettings->Register(algo);
                      MOGTLDAlgoSettings->DisplaySettings();
                      break;
                  default:
                      break;
                  }
              }
          }
          else if(VisionUnitWidgetsMap[ deviceNumber ])
          {
              VisionUnitWidgetsMap[ deviceNumber ]->Play(fileName);
              VisionUnitWidgetsMap[ deviceNumber ]->SetVisionAlgorithm( ui->algoComboBox->currentText() );
          }
      }
      else if(cameraConnectDialog->isIPCameraUsed())
      {
          URL = cameraConnectDialog->GetIPCameraURL();

          if(!URL.isEmpty())
          {
              // TODO: get this from the IP cam
              deviceNumber += IpCamCounter;
              ++IpCamCounter;
          }
      }
      // Check if this camera is already connected
      if( !DeviceNumbersMap.contains(deviceNumber) && 
          !cameraConnectDialog->isVideoUsed() )
      {
          QApplication::setOverrideCursor(Qt::WaitCursor);

          // Create ImageBuffer with user-defined size
/*          ImageBuffer *imageBuffer = new ImageBuffer( cameraConnectDialog->getImageBufferSize() );*/

          // Add created ImageBuffer to SharedImageBuffer object
          SImageBuffer->add( deviceNumber,
                             new ImageBuffer( cameraConnectDialog->getImageBufferSize() ) );

          VisionUnitsMap[ deviceNumber ] = new VisionUnit( deviceNumber,
                                                           SImageBuffer,
                                                           cameraConnectDialog->isChameleonUsed(),
                                                           URL );

          // If everything went well, the stream should exists and be connected
          if( VisionUnitsMap[ deviceNumber ]->CameraConnected() )
          {
              // Add a vision unit UI
              VisionUnitWidgetsMap[ deviceNumber ] = new VisionUnit_widget(ui->tabWidget);

              // Connect the Vision Unit to its UI
              connect( VisionUnitsMap[ deviceNumber ]->GetProcessingThread(),
                       SIGNAL(newFrame(QImage)),
                       VisionUnitWidgetsMap[ deviceNumber ],
                       SLOT(UpdateFrame(QImage)) );

              connect( VisionUnitWidgetsMap[ deviceNumber ],
                       SIGNAL(setROI(QRect)),
                       VisionUnitsMap[ deviceNumber ]->GetProcessingThread(),
                       SLOT(setROI(QRect)));

              connect( VisionUnitsMap[ deviceNumber ]->GetProcessingThread(),
                       SIGNAL(updateStatisticsInGUI(struct ThreadStatisticsData)),
                       VisionUnitWidgetsMap[ deviceNumber ],
                       SLOT(updateProcessingThreadStats(struct ThreadStatisticsData)));

              // TODO: some other stuff remain to be connected to the UI

              QString tabLabel = cameraConnectDialog->getTabLabel();

              if(DeviceNumbersMap.size() == 0)
              {
                  DeviceNumbersMap    [ deviceNumber ] = 0;

                  ui->tabWidget->removeTab(0);
                  ui->tabWidget->insertTab(0, VisionUnitWidgetsMap[ deviceNumber ], tabLabel + " [" + QString::number(deviceNumber) + "]");
              }
              else
              {
                  DeviceNumbersMap    [ deviceNumber ] = nextTabIndex;

                  ui->tabWidget->addTab(VisionUnitWidgetsMap[ deviceNumber ], tabLabel + " [" + QString::number(deviceNumber) + "]");
              }


              ui->tabWidget->setCurrentWidget(VisionUnitWidgetsMap[ deviceNumber ]);

              // Set the Vision Unit's current vision algorithm
              AlgoVision* algo = VisionUnitsMap[ deviceNumber ]->SetVisionAlgorithm( ui->algoComboBox->currentText() );
              if(algo)
              {
                  // Connect the algo to its corresponding UI
                  int idx = ui->algoComboBox->currentIndex();

                  switch(idx) {
                  case 0 :
                      TLDAlgoSettings->Register(algo);
                      TLDAlgoSettings->DisplaySettings();
                      break;
                  case 1 :
                      MOGAlgoSettings->Register(algo);
                      MOGAlgoSettings->DisplaySettings();
                      break;
                  case 2 :
                      MOGTLDAlgoSettings->Register(algo);
                      MOGTLDAlgoSettings->DisplaySettings();
                      break;
                  default:
                      break;
                  }
              }
              // We are done, restore override cursor
              QApplication::restoreOverrideCursor();
          }
          else
          {
              delete VisionUnitsMap[ deviceNumber ];
              VisionUnitsMap.remove( deviceNumber );

              SImageBuffer->removeByDeviceNumber( deviceNumber );
/*              delete imageBuffer;*/

              QApplication::restoreOverrideCursor();

              QMessageBox::warning( this, "ERROR:", "Could not connect to camera." );
          }
      }
      else if(!cameraConnectDialog->isVideoUsed())
      {
          QMessageBox::warning( this, "ERROR:", "Could not connect to camera. Already connected" );
      }
   }

  delete cameraConnectDialog;
}


//////////////////////////////////////////////////////////////////

bool MainWindow::removeFromMapByTabIndex(QMap<int, VisionUnit*> &map, int tabIndex)
{
    QMap<int, VisionUnit*>::iterator i = map.begin();
    while (i != map.end())
    {
         if(i.key() == tabIndex)
         {
             i = map.erase(i);
             return true;
         }
         i++;
    }
    return false;
}


//////////////////////////////////////////////////////////////////

bool MainWindow::removeFromMapByTabIndex(QMap<int, VisionUnit_widget *> &map, int tabIndex)
{
    QMap<int, VisionUnit_widget*>::iterator i = map.begin();
    while (i != map.end())
    {
         if(i.key() == tabIndex)
         {
             i = map.erase(i);
             return true;
         }
         i++;
    }
    return false;
}


//////////////////////////////////////////////////////////////////

bool MainWindow::removeFromMapByTabIndex(QMap<int, int> &map, int tabIndex)
{
    QMap<int, int>::iterator i = map.begin();
    while (i != map.end())
    {
         if(i.key() == tabIndex)
         {
             i = map.erase(i);
             return true;
         }
         i++;
    }
    return false;
}


//////////////////////////////////////////////////////////////////

void MainWindow::updateMapValues(QMap<int, int> &map, int tabIndex)
{
    QMutableMapIterator<int, int> i(map);
    while (i.hasNext())
    {
        i.next();
        if(i.value()>tabIndex)
            i.setValue(i.value()-1);
    }
}


//////////////////////////////////////////////////////////////////

void MainWindow::on_algoComboBox_currentIndexChanged(const QString &arg1)
{
    int index = ui->tabWidget->currentIndex();

    if(VisionUnitsMap[ DeviceNumbersMap.key( index ) ])
    {
        AlgoVision* algo = VisionUnitsMap[ DeviceNumbersMap.key( index ) ]->GetVisionAlgorithm();

        if ( !algo || !( VisionUnitsMap[ DeviceNumbersMap.key( index ) ]->GetVisionAlgorithmName() == arg1 ) )
        {
            algo = VisionUnitsMap[ DeviceNumbersMap.key( index ) ]->SetVisionAlgorithm( arg1 );
        }
        if( algo )
        {
            int idx = ui->algoComboBox->currentIndex();

            switch(idx) {
            case 0 :
                TLDAlgoSettings->Register(algo);
                TLDAlgoSettings->DisplaySettings();
                break;
            case 1 :
                MOGAlgoSettings->Register(algo);
                MOGAlgoSettings->DisplaySettings();
                break;
            case 2 :
                MOGTLDAlgoSettings->Register(algo);
                MOGTLDAlgoSettings->DisplaySettings();
                break;
            default:
                break;
            }
        }
    }
    else if(IsVideoOpened && VisionUnitWidgetsMap[ DeviceNumbersMap.key( index ) ])
    {
        // Set the Vision Unit's current vision algorithm
        AlgoVision* algo = VisionUnitWidgetsMap[ DeviceNumbersMap.key( index ) ]->SetVisionAlgorithm( arg1 );
        if( algo )
        {
            // Connect the algo to its corresponding UI
            int idx = ui->algoComboBox->currentIndex();

            switch(idx) {
            case 0 :
                TLDAlgoSettings->Register(algo);
                TLDAlgoSettings->DisplaySettings();
                break;
            case 1 :
                MOGAlgoSettings->Register(algo);
                MOGAlgoSettings->DisplaySettings();
                break;
            case 2 :
                MOGTLDAlgoSettings->Register(algo);
                MOGTLDAlgoSettings->DisplaySettings();
                break;
            default:
                break;
            }
        }
    }
}


//////////////////////////////////////////////////////////////////

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(VisionUnitsMap[ DeviceNumbersMap.key( index ) ])
    {
        // Get the index corresponding Vision Unit algorithm name
        QString algoName = VisionUnitsMap[ DeviceNumbersMap.key( index ) ]->GetVisionAlgorithmName();

        int idx = ui->algoComboBox->findText(algoName);
        ui->algoComboBox->setCurrentIndex(idx);

        // TODO: change Display Settings and Algo Settings
        StackedWidget->setCurrentIndex(idx);

        switch(idx) {
        case 0 :
            TLDAlgoSettings->DisplaySettings();
            break;
        case 1 :
            MOGAlgoSettings->DisplaySettings();
            break;
        case 2 :
            MOGTLDAlgoSettings->DisplaySettings();
            break;
        default:
            break;
        }
    }
}
