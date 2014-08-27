/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* ProcessingThread.h                                                   */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/*                                                                      */
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

#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

// Qt
#include <QThread>
#include <QDebug>
#include <QTime>
#include <QImage>
// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
// Local
#include "Structures.h"
#include "Config.h"
#include "ImageBuffer.h"
#include "SharedImageBuffer.h"
#include "Vision/AlgoVision.h"

using namespace cv;

class ProcessingThread : public QThread
{
    Q_OBJECT

    public:
        ProcessingThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber, QString algoName = "TLD Tracking" );
        ~ProcessingThread();
        QRect getCurrentROI();
        AlgoVision *changeAlgo(QString algoName);
        AlgoVision *GetAlgo();
        void stop();
        int GetDeviceNumber();
        void SetTabIdx( int idx );

    private:
        SharedImageBuffer *sharedImageBuffer;
        Mat currentFrame;
        Mat currentFrameGrayscale;
        QImage frame;
        QMutex doStopMutex;
        QMutex processingMutex;
        Size frameSize;
        Point framePoint;
        struct ThreadStatisticsData statsData;
        volatile bool doStop;
        int sampleNumber;
        int deviceNumber;
        bool enableFrameProcessing;
        AlgoVision* algo;
        int TabIdx;
        CvRect currentROI;
        QQueue<int> fps;
        int processingTime;
        int fpsSum;
        QTime t;

        QImage MatToQImage(const Mat&);
        void updateFPS(int);

    protected:
        void run();

    private slots:
        void setROI(QRect roi);

    signals:
        void newFrame(const QImage &frame, int tabIdx = 0);
        void updateStatisticsInGUI(struct ThreadStatisticsData);
};

#endif // PROCESSINGTHREAD_H
