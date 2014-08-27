/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* ProcessingThread.cpp                                                 */
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

#include "ProcessingThread.h"

#include "Vision/SentryMoG.h"
#include "Vision/Calibration.h"
#include "Vision/SentryTLD.h"
#include "Vision/MoGTLD.h"


//////////////////////////////////////////////////////////////////

ProcessingThread::ProcessingThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber, QString algoName) :
    QThread(),
    sharedImageBuffer(sharedImageBuffer),
    TabIdx( 0 )
{
    // Save Device Number
    this->deviceNumber = deviceNumber;

    // Initialize members
    doStop = false;
    sampleNumber = 0;

    // Init the bounding box (AKA ROI)
    currentROI.height = 0;
    currentROI.width = 0;

    currentROI.x = 0;
    currentROI.y = 0;

    algo = 0;

    fpsSum = 0;
    fps.clear();
    statsData.averageFPS = 0;
    statsData.nFramesProcessed = 0;

    changeAlgo(algoName);
}


//////////////////////////////////////////////////////////////////

void ProcessingThread::run()
{
    while(1)
    {
        ////////////////////////////////
        // Stop thread if doStop=TRUE //
        ////////////////////////////////
        doStopMutex.lock();

        if(doStop)
        {
            doStop = false;
            doStopMutex.unlock();
            break;
        }

        doStopMutex.unlock();
        /////////////////////////////////
        /////////////////////////////////

        // Save processing time
        processingTime=t.elapsed();
        // Start timer (used to calculate processing rate)
        t.start();

        processingMutex.lock();

        // Get frame from queue, store in currentFrame, set ROI
        currentFrame = Mat( sharedImageBuffer->getByDeviceNumber(deviceNumber)->getFrame() );

        if(algo)
            algo->execute( currentFrame, &currentROI );

        // Convert Mat to QImage
        frame = MatToQImage( currentFrame );

        processingMutex.unlock();

        // Inform GUI thread of new frame (QImage)
        emit newFrame( frame, TabIdx );

        // Update statistics
        updateFPS(processingTime);
        statsData.nFramesProcessed++;

        // Inform GUI of updated statistics
        emit updateStatisticsInGUI(statsData);
    }

    qDebug() << "Stopping processing thread...";
}


//////////////////////////////////////////////////////////////////

void ProcessingThread::stop()
{
    QMutexLocker locker(&doStopMutex);
    doStop=true;
}


//////////////////////////////////////////////////////////////////

AlgoVision* ProcessingThread::changeAlgo(QString name)
{
    processingMutex.lock();

    if( name == "TLD Tracking" )
    {
        if(algo)
            delete algo;

        algo = new SentryTLD();
    }
    else if( name == "MOG Tracking" )
    {
        if(algo)
            delete algo;

        algo = new SentryMoG();
    }
    else if( name == "MOGTLD Tracking" )
    {
        if(algo)
            delete algo;

        algo = new MoGTLD();
    }

    processingMutex.unlock();

    return algo;
}


//////////////////////////////////////////////////////////////////

AlgoVision* ProcessingThread::GetAlgo()
{
    return algo;
}


//////////////////////////////////////////////////////////////////

QImage ProcessingThread::MatToQImage(const Mat& mat)
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
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}


//////////////////////////////////////////////////////////////////

int ProcessingThread::GetDeviceNumber()
{
    return deviceNumber;
}


//////////////////////////////////////////////////////////////////

void ProcessingThread::SetTabIdx( int idx )
{
    TabIdx = idx;
}


//////////////////////////////////////////////////////////////////

void ProcessingThread::setROI(QRect roi)
{
    QMutexLocker locker(&processingMutex);
    currentROI.x = roi.x();
    currentROI.y = roi.y();
    currentROI.width = roi.width();
    currentROI.height = roi.height();
}


//////////////////////////////////////////////////////////////////

void ProcessingThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fps.enqueue((int)1000/timeElapsed);
        // Increment sample number
        sampleNumber++;
    }

    // Maximum size of queue is DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH
    if(fps.size()>PROCESSING_FPS_STAT_QUEUE_LENGTH)
        fps.dequeue();

    // Update FPS value every DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH samples
    if((fps.size()==PROCESSING_FPS_STAT_QUEUE_LENGTH)&&(sampleNumber==PROCESSING_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fps.empty())
            fpsSum+=fps.dequeue();
        // Calculate average FPS
        statsData.averageFPS=fpsSum/PROCESSING_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum=0;
        // Reset sample number
        sampleNumber=0;
    }
}


//////////////////////////////////////////////////////////////////

ProcessingThread::~ProcessingThread()
{
    delete algo;
}
