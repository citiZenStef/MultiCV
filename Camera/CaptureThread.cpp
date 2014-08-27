/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* CaptureThread.cpp                                                    */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/* Adaptation from:                                                     */
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* CaptureThread.cpp                                                    */
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
#include "CaptureThread.h"



//////////////////////////////////////////////////////////////////

CaptureThread::CaptureThread( SharedImageBuffer *sharedImageBuffer,
                              int deviceNumber,
                              QString URL, bool dropFrameIfBufferFull ) :
    QThread(),
    SImageBuffer(sharedImageBuffer),
    DeviceNumber(deviceNumber),
    DropFrameIfBufferFull(dropFrameIfBufferFull),
    DoStop(false),
    Cap(0),
    IsConnected( false ),
    URL(URL)
{
    frameH = 0;  
    frameW = 0;  
    fps = 0;     
    numFrame = 0;
}


//////////////////////////////////////////////////////////////////

CaptureThread::~CaptureThread()
{
    if(Cap)
        delete Cap;
}


//////////////////////////////////////////////////////////////////

void CaptureThread::run()
{
    while(1)
    {
        ////////////////////////////////
        // Stop thread if doStop=TRUE //
        ////////////////////////////////
        DoStopMutex.lock();
        if(DoStop)
        {
            DoStop=false;

            DoStopMutex.unlock();
            break;
        }
        DoStopMutex.unlock();
        /////////////////////////////////
        /////////////////////////////////

        if(fps)
            msleep(1000/fps);

        // Capture and add frame to buffer
        if( Cap->grab() )
        {
            Cap->retrieve( GrabbedFrame );
            SImageBuffer->getByDeviceNumber(DeviceNumber)->addFrame( GrabbedFrame,
                                                                     DropFrameIfBufferFull );
        }
        else
        {
            DoStop = true;

            if(SImageBuffer->getByDeviceNumber( DeviceNumber ))
                SImageBuffer->getByDeviceNumber( DeviceNumber )->clear();
        }
    }

    qDebug() << "Stopping capture thread...";
}


//////////////////////////////////////////////////////////////////

bool CaptureThread::ConnectToCamera( bool useChameleon )
{
    if( !Cap )
    {
        if(!URL.isEmpty())
            Cap = new VideoCapture( URL.toStdString() );
        else
            Cap = new VideoCapture(DeviceNumber);
    }

    if(IsConnected = (Cap && Cap->isOpened()))
    {
        frameH   = (int) Cap->get(CV_CAP_PROP_FRAME_HEIGHT);
        frameW   = (int) Cap->get(CV_CAP_PROP_FRAME_WIDTH);
        fps      = (int) Cap->get(CV_CAP_PROP_FPS);
    }

    return IsConnected;
}


//////////////////////////////////////////////////////////////////

bool CaptureThread::DisconnectCap()
{
    if( Cap )
    {
        // Disconnect camera
        Cap->release();

        return true;
    }
    // Camera is NOT connected
    else
        return false;
}


//////////////////////////////////////////////////////////////////

void CaptureThread::Stop()
{
    QMutexLocker locker( &DoStopMutex );

    DoStop = true;
}


//////////////////////////////////////////////////////////////////

bool CaptureThread::isCapOpened()
{
    return IsConnected;
}


//////////////////////////////////////////////////////////////////

int CaptureThread::GetDeviceNumber()
{
    return DeviceNumber;
}


//////////////////////////////////////////////////////////////////

bool CaptureThread::OpenVideoFile( const QString fileName )
{
    if( !Cap )
        Cap = new VideoCapture();

    std::string utf8_text = fileName.toUtf8().constData();

    if( Cap->open( utf8_text.c_str() ) )
    {
        frameH   = (int) Cap->get(CV_CAP_PROP_FRAME_HEIGHT);
        frameW   = (int) Cap->get(CV_CAP_PROP_FRAME_WIDTH);
        fps      = (int) Cap->get(CV_CAP_PROP_FPS);
        numFrame = (int) Cap->get(CV_CAP_PROP_POS_FRAMES);

        IsConnected = true;
    }
    
    return IsConnected;
}


//////////////////////////////////////////////////////////////////
