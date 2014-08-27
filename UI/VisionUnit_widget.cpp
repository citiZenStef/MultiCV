/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* VisionUnit_widget.cpp                                                */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#include "VisionUnit_widget.h"
#include "ui_VisionUnit_widget.h"
// Qt
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>

#include "Vision/SentryMoG.h"
#include "Vision/Calibration.h"
#include "Vision/SentryTLD.h"
#include "Vision/MoGTLD.h"

VisionUnit_widget::VisionUnit_widget(QWidget *parent, QString filename) :
  QWidget(parent),
  ui( new Ui::VisionUnit_widget ),
  VideoCap(0),
  READCOUNT(0),
  FPS_open(0),
  NumFrames(0),
  FrameW(0),
  FrameH(0),
  IsVideoPause(false)
{
    if( ui )
    {
        ui->setupUi(this);
        ui->frameLabel->setText( "No camera connected" );
        
        if(!filename.isEmpty())
        {
            FileName = filename;
        
            TimerOpen = new QTimer(this);
            connect(TimerOpen,SIGNAL(timeout()),this,SLOT(Read()));
        
            connect(ui->videoSlider, SIGNAL(sliderPressed()),  this, SLOT(SliderFramePress()));
            connect(ui->videoSlider, SIGNAL(sliderReleased()), this, SLOT(SliderFrameRelease()));
            connect(ui->videoSlider, SIGNAL(sliderMoved(int)), this, SLOT(SliderFrameMove(int)));
        
            connect(ui->playPushButton,  SIGNAL(clicked()), this, SLOT(Play()));
            connect(ui->pausePushButton, SIGNAL(clicked()), this, SLOT(Pause()));
            connect(ui->stopPushButton,  SIGNAL(clicked()), this, SLOT(Stop()));

            connect( this, SIGNAL(setROI(QRect)), this, SLOT(setROI_slot(QRect)));

            VideoAlgoVision = 0;

            CurrentROI.x = 0;
            CurrentROI.y = 0;
            CurrentROI.width = 0;
            CurrentROI.height = 0;
        }
        else
        {
            ui->playPushButton ->setEnabled(false);
            ui->pausePushButton->setEnabled(false);
            ui->stopPushButton ->setEnabled(false);
            ui->ffdPushButton  ->setEnabled(false);
            ui->rewPushButton  ->setEnabled(false);
            ui->videoSlider    ->setEnabled(false);
        }
                    
        connect(ui->frameLabel, SIGNAL(onMouseMoveEvent()), this, SLOT(UpdateMouseCursorPosLabel()));
        connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));    
    }
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::UpdateFrame(const QImage &frame)
{
    ui->frameLabel->setFixedWidth(frame.width());
    ui->frameLabel->setFixedHeight(frame.height());

    // Display frame
    ui->frameLabel->setPixmap(QPixmap::fromImage(frame));
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::UpdateMouseCursorPosLabel()
{
    // Update mouse cursor position in mouseCursorPosLabel
    ui->mouseCursorPosLabel->setText(QString("(")+QString::number(ui->frameLabel->getMouseCursorPos().x())+
                                     QString(",")+QString::number(ui->frameLabel->getMouseCursorPos().y())+
                                     QString(")"));
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::updateProcessingThreadStats(struct ThreadStatisticsData statData)
{
    // Show processing rate in processingRateLabel
    ui->processingRateLabel->setText(QString::number(statData.averageFPS)+" fps");
    // Show number of frames processed in nFramesProcessedLabel
    ui->nFramesProcessedLabel->setText(QString("[") + QString::number(statData.nFramesProcessed) + QString("]"));
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::newMouseData(struct MouseData mouseData)
{
    // Local variable(s)
    int x_temp, y_temp, width_temp, height_temp;

    // Set ROI
    if(mouseData.leftButtonRelease)
    {
        double xScalingFactor;
        double yScalingFactor;
        double wScalingFactor;
        double hScalingFactor;

        xScalingFactor=(double) mouseData.selectionBox.x() / (double) ui->frameLabel->width();
        yScalingFactor=(double) mouseData.selectionBox.y() / (double) ui->frameLabel->height();

        // Set selection box properties (new ROI)
        selectionBox.setX(mouseData.selectionBox.x());
        selectionBox.setY(mouseData.selectionBox.y());
        selectionBox.setWidth(mouseData.selectionBox.width());
        selectionBox.setHeight(mouseData.selectionBox.height());

        // Check if selection box has NON-ZERO dimensions
        if((selectionBox.width()!=0)&&((selectionBox.height())!=0))
        {
            // Selection box can also be drawn from bottom-right to top-left corner
            if(selectionBox.width()<0)
            {
                x_temp=selectionBox.x();
                width_temp=selectionBox.width();
                selectionBox.setX(x_temp+selectionBox.width());
                selectionBox.setWidth(width_temp*-1);
            }
            if(selectionBox.height()<0)
            {
                y_temp=selectionBox.y();
                height_temp=selectionBox.height();
                selectionBox.setY(y_temp+selectionBox.height());
                selectionBox.setHeight(height_temp*-1);
            }

            // Check if selection box is not outside window
            if((selectionBox.x()<0)||(selectionBox.y()<0))
            {
                // Display error message
                QMessageBox::warning(this,"ERROR:","Selection box outside range. Please try again.");
            }
            // Set ROI
            else if(selectionBox.isValid())
                emit setROI(selectionBox);
        }
    }
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::Play()
{
    if(IsVideoPause)
    {
        ui->pausePushButton->setEnabled(true);
        ui->playPushButton->setEnabled(false);
        IsVideoPause = false;
        TimerOpen->start( (int) (1000/FPS_open) );

        return;
    }
    else
    {
        ui->pausePushButton->setEnabled(true);
        ui->stopPushButton ->setEnabled(true);
        ui->rewPushButton  ->setEnabled(true);
        ui->ffdPushButton  ->setEnabled(true);
        ui->videoSlider    ->setEnabled(true);

        ui->playPushButton ->setEnabled(false);

        if(!VideoCap)
            VideoCap = new VideoCapture( FileName.toStdString() );
        else
            VideoCap->open( FileName.toStdString() );

        if( VideoCap->isOpened() )
        {
            FrameH    = (int) VideoCap->get(CV_CAP_PROP_FRAME_HEIGHT);
            FrameW    = (int) VideoCap->get(CV_CAP_PROP_FRAME_WIDTH);
            FPS_open  = (int) VideoCap->get(CV_CAP_PROP_FPS);
            NumFrames = (int) VideoCap->get(CV_CAP_PROP_FRAME_COUNT);

            ui->videoSlider->setMaximum( (int)NumFrames );

            ui->videoSlider->setEnabled(true);
            READCOUNT = 0;
            TimerOpen->start( (int) (1000/FPS_open) );
        }
    }   
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::Play(QString& filename)
{
    Q_ASSERT(!filename.isEmpty());

    if(!filename.isEmpty())
    {
        TimerOpen = new QTimer(this);
        connect(TimerOpen,SIGNAL(timeout()),this,SLOT(Read()));

        VideoAlgoVision = 0;

        CurrentROI.x = 0;
        CurrentROI.y = 0;
        CurrentROI.width = 0;
        CurrentROI.height = 0;

        ui->pausePushButton->setEnabled(true);
        ui->stopPushButton ->setEnabled(true);
        ui->rewPushButton  ->setEnabled(true);
        ui->ffdPushButton  ->setEnabled(true);
        ui->videoSlider    ->setEnabled(true);

        ui->playPushButton ->setEnabled(true);

        if(!VideoCap)
            VideoCap = new VideoCapture( filename.toStdString() );

        if( VideoCap->isOpened() )
        {
            FrameH    = (int) VideoCap->get(CV_CAP_PROP_FRAME_HEIGHT);
            FrameW    = (int) VideoCap->get(CV_CAP_PROP_FRAME_WIDTH);
            FPS_open  = (int) VideoCap->get(CV_CAP_PROP_FPS);
            NumFrames = (int) VideoCap->get(CV_CAP_PROP_FRAME_COUNT);

            ui->videoSlider->setMaximum( (int)NumFrames );

            ui->videoSlider->setEnabled(true);
            READCOUNT = 0;
            TimerOpen->start( (int) (1000/(FPS_open + 1)) );
        }
    }
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::Pause()
{
    ui->playPushButton->setEnabled(true);
    ui->pausePushButton->setEnabled(false);
    TimerOpen->stop();
    IsVideoPause = true;
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::Stop()
{
    ui->stopPushButton->setEnabled(false);
    ui->playPushButton->setEnabled(false);
    ui->pausePushButton->setEnabled(false);
    ui->rewPushButton->setEnabled(false);
    ui->ffdPushButton->setEnabled(false);

    FileName = "";

    TimerOpen->stop();

    READCOUNT = 0;

    ui->videoSlider->setSliderPosition(0);
    ui->videoSlider->setEnabled(false);

    ui->frameLabel->setText( "No camera connected" );

    delete TimerOpen;
    TimerOpen = 0;

    if(VideoCap)
    {
        delete VideoCap;
        VideoCap = 0;
    }

    if(VideoAlgoVision)
    {
        delete VideoAlgoVision;
        VideoAlgoVision = 0;
    }
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::Read()
{
    READCOUNT++;

    // Update Video Player's slider
    ui->videoSlider->setValue(READCOUNT);

    if(READCOUNT >= NumFrames) // if avi ends
    {
        Pause();
        return;
    }

    Mat grabbedFrame;

    
    // Get next frame
    if(VideoCap->isOpened() && VideoCap->read(grabbedFrame))
    {
        // Execute the vision filter
        if(VideoAlgoVision)
            VideoAlgoVision->execute( grabbedFrame, &CurrentROI );
    
        // Convert Mat to QImage
        QImage frame = MatToQImage( grabbedFrame );

        // Update the display
        UpdateFrame( frame );
    }
}


//////////////////////////////////////////////////////////////////

QImage VisionUnit_widget::MatToQImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;

        for (int i=0; i<256; i++)
        {
            colorTable.push_back(qRgb(i,i,i));
        }

        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;

        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);

        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    else if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;

        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

        return img.rgbSwapped();
    }
    else
    {
        return QImage();
    }
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::SliderFramePress()
{
    TimerOpen->stop();
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::SliderFrameRelease()
{
    ui->pausePushButton ->setEnabled(true);
    ui->playPushButton ->setEnabled(false);
    IsVideoPause = false;

    TimerOpen->start((int) (1000/FPS_open));
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::SliderFrameMove(int k)
{
    VideoCap->set(CV_CAP_PROP_POS_FRAMES,k);

    Mat grabbedFrame;

    VideoCap->retrieve( grabbedFrame );

    // Convert Mat to QImage
    QImage frame = MatToQImage( grabbedFrame );

    UpdateFrame( frame );

    READCOUNT = k;
}


//////////////////////////////////////////////////////////////////

AlgoVision* VisionUnit_widget::SetVisionAlgorithm( const QString& algorithmName )
{
    Q_ASSERT(!algorithmName.isEmpty());

    if(!algorithmName.isEmpty())
    {
        if( algorithmName == "TLD Tracking" )
        {
            if(VideoAlgoVision)
                delete VideoAlgoVision;

            VideoAlgoVision = new SentryTLD();
        }
        else if( algorithmName == "MOG Tracking" )
        {
            if(VideoAlgoVision)
                delete VideoAlgoVision;

            VideoAlgoVision = new SentryMoG();
        }
        else if( algorithmName == "MOGTLD Tracking" )
        {
            if(VideoAlgoVision)
                delete VideoAlgoVision;

            VideoAlgoVision = new MoGTLD();
        }
    }

    return VideoAlgoVision;
}


//////////////////////////////////////////////////////////////////

void VisionUnit_widget::setROI_slot(QRect roi)
{
    CurrentROI.x = roi.x();
    CurrentROI.y = roi.y();
    CurrentROI.width = roi.width();
    CurrentROI.height = roi.height();
}