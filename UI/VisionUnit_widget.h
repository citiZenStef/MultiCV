/************************************************************************/
/* MultiCV software:                                           */
/* Computer vision                                                      */
/*                                                                      */
/* VisionUnit_widget.h                                                  */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#ifndef VISIONUNIT_WIDGET_H
#define VISIONUNIT_WIDGET_H

#include <QWidget>
#include <QtCore/QRect>
#include <QTimer>

#include "opencv/highgui.h"

#include "Camera/Structures.h"

#include "Vision/AlgoVision.h"

using namespace cv;
using namespace std;

//////////////////////////////////////////////////////////////////

namespace Ui {
  class VisionUnit_widget;
}

//////////////////////////////////////////////////////////////////

class VisionUnit_widget : public QWidget
{
  Q_OBJECT
  
public:
    explicit VisionUnit_widget(QWidget *parent, QString filename = "");

    AlgoVision *SetVisionAlgorithm( const QString& algorithmName );

public slots:
    void UpdateMouseCursorPosLabel();
    void newMouseData(struct MouseData mouseData);
  
private:
    Ui::VisionUnit_widget *ui;
    QRect selectionBox;

    // Video Player related
    VideoCapture* VideoCap;
    Mat* GrabbedFrame;
    AlgoVision* VideoAlgoVision;
    int READCOUNT;
    int FPS_open;
    int NumFrames;
    int FrameW;
    int FrameH;
    QTimer* TimerOpen;
    QString FileName;
    bool IsVideoPause;
    CvRect CurrentROI;

    QImage MatToQImage(const Mat& mat);

public:
    void Play(QString& filename);

private slots:
    void UpdateFrame( const QImage& frame );
    void updateProcessingThreadStats(struct ThreadStatisticsData statData);    
    void Pause();
    void Play();
    void Stop();
    void Read();
    void SliderFramePress();
    void SliderFrameRelease();
    void SliderFrameMove(int k);
    void setROI_slot(QRect roi);

signals:
    void setROI(QRect roi);

};

#endif // VISIONUNIT_WIDGET_H
