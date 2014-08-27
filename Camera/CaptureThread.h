/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* CaptureThread.h                                                      */
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
#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H


#include <QThread>
#include <QTime>
#include <QDebug>

#include "opencv/highgui.h"

#include "SharedImageBuffer.h"
#include "Config.h"
#include "Structures.h"

using namespace cv;

//////////////////////////////////////////////////////////////////

// Forward declaration
class ImageBuffer;
class Chameleon;


//////////////////////////////////////////////////////////////////

class CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread( SharedImageBuffer *sharedImageBuffer,
                   int deviceNumber = 0,
                   QString URL = "",
                   bool dropFrameIfBufferFull = false );

    ~CaptureThread();

    void Stop();

    bool ConnectToCamera( bool useChameleon );
    bool ConnectToChameleonCamera();

    bool DisconnectCap();
    bool isCapOpened();

    int GetDeviceNumber();

    bool OpenVideoFile( const QString fileName );

protected:
    void run();

private:
    Chameleon* CamChameleon;
    VideoCapture* Cap;
    Mat GrabbedFrame;
    QMutex DoStopMutex;
    int DeviceNumber;
    bool DropFrameIfBufferFull;
    volatile bool DoStop;
    SharedImageBuffer* SImageBuffer;
    bool IsConnected;
    QString URL;

    int frameH;  
    int frameW;  
    int fps;     
    int numFrame;
};

#endif // CAPTURETHREAD_H
