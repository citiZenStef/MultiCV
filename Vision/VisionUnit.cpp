#include "VisionUnit.h"


//////////////////////////////////////////////////////////////////

VisionUnit::VisionUnit(int deviceNumber,
                       SharedImageBuffer* shImgbuffer,
                       bool useChameleon /* = false */, QString videoStreamName /* = "" */) :
  ProcThread( 0 ),
  CaptThread( 0 ),
  IsCameraConnected( false ),
  DeviceNumber( deviceNumber ),
  ShrImageBuffer( shImgbuffer ),
  UseChameleonCamera( useChameleon ),
  VideoFileName( videoStreamName ),
  AlgoName("TLD Tracking")
{
    IsCameraConnected = Initialize();

    if(IsCameraConnected && ProcThread)
        ProcThread->start();
}


//////////////////////////////////////////////////////////////////

VisionUnit::VisionUnit(int deviceNumber, QString URL, SharedImageBuffer *shImgbuffer) :
    ProcThread( 0 ),
    CaptThread( 0 ),
    IsCameraConnected( false ),
    DeviceNumber( deviceNumber ),
    ShrImageBuffer( shImgbuffer ),
    UseChameleonCamera( false ),
    VideoFileName( "" ),
    IpCameraURL(URL)
{
    IsCameraConnected = Initialize();

    if(IsCameraConnected && ProcThread)
        ProcThread->start();
}


//////////////////////////////////////////////////////////////////

VisionUnit::~VisionUnit()
{
  if(IsCameraConnected)
  {
      if(ProcThread)
      {
          if( ProcThread->isRunning() )
            StopProcessingThread();

          delete ProcThread;
          ProcThread = 0;
      }

      if(CaptThread)
      {
          if( CaptThread->isRunning() )
            StopCaptureThread();

          ShrImageBuffer->removeByDeviceNumber( DeviceNumber );

          if( CaptThread->DisconnectCap() )
            qDebug() << "[" << DeviceNumber << "] Camera successfully disconnected";

          delete CaptThread;

          CaptThread = 0;
      }
  }
}


//////////////////////////////////////////////////////////////////

void VisionUnit::SetVideoFileName(const QString &videoName)
{
  VideoFileName = videoName;
}


//////////////////////////////////////////////////////////////////

bool VisionUnit::Initialize()
{
  CaptThread = new CaptureThread( ShrImageBuffer, DeviceNumber, IpCameraURL );

  if( !VideoFileName.isEmpty() )
  {
      // todo CaptThread connect to video
      CaptThread->OpenVideoFile( VideoFileName );
      ProcThread = new ProcessingThread( ShrImageBuffer, DeviceNumber );
  }
  else if( CaptThread->ConnectToCamera( UseChameleonCamera ) )
  {
      ProcThread = new ProcessingThread( ShrImageBuffer, DeviceNumber );     
  }

  CaptThread->start();

  return CaptThread->isCapOpened();
}


//////////////////////////////////////////////////////////////////

const ProcessingThread* VisionUnit::GetProcessingThread() const
{
  return ProcThread;
}


//////////////////////////////////////////////////////////////////

const bool VisionUnit::CameraConnected() const
{
  return IsCameraConnected;
}


//////////////////////////////////////////////////////////////////

void VisionUnit::StopCaptureThread()
{
    if( CaptThread->isRunning() )
    {
        int deviceNumber = CaptThread->GetDeviceNumber();

        qDebug() << "[" << deviceNumber << "] About to stop capture thread...";
        CaptThread->Stop();

        // This allows the thread to be stopped if it is in a wait-state
        ShrImageBuffer->wakeAll();

        // Take one frame off a FULL queue to allow the capture thread to finish
        if( ShrImageBuffer->getByDeviceNumber( deviceNumber )->isFull() )
        {
            ShrImageBuffer->getByDeviceNumber( deviceNumber )->getFrame();
        }

        CaptThread->wait();

        qDebug() << "[" << deviceNumber << "] Capture thread successfully stopped.";
    }
}


//////////////////////////////////////////////////////////////////

void VisionUnit::StopProcessingThread()
{
    if( ProcThread->isRunning() )
    {
        qDebug() << "[" << ProcThread->GetDeviceNumber() << "] About to stop processing thread...";

        ProcThread->stop();

        ProcThread->wait( 1000 );

        qDebug() << "[" << ProcThread->GetDeviceNumber() << "] Processing thread successfully stopped.";
    }
}


//////////////////////////////////////////////////////////////////

AlgoVision* VisionUnit::SetVisionAlgorithm(const QString &algorithmName)
{
    Q_ASSERT( ProcThread );

    AlgoName = algorithmName;
    return ProcThread->changeAlgo( AlgoName );
}


//////////////////////////////////////////////////////////////////

AlgoVision* VisionUnit::GetVisionAlgorithm()
{
    return ProcThread->GetAlgo();
}


//////////////////////////////////////////////////////////////////

QString VisionUnit::GetVisionAlgorithmName()
{
    return AlgoName;
}


//////////////////////////////////////////////////////////////////

bool VisionUnit::Terminate()
{
  if(IsCameraConnected)
  {
      if(ProcThread)
      {
          if( ProcThread->isRunning() )
            StopProcessingThread();

          delete ProcThread;
          ProcThread = 0;
      }

      if(CaptThread)
      {
          if( CaptThread->isRunning() )
            StopCaptureThread();

          ShrImageBuffer->removeByDeviceNumber( DeviceNumber );

          if( CaptThread->DisconnectCap() )
            qDebug() << "[" << DeviceNumber << "] Camera successfully disconnected";

          delete CaptThread;
          CaptThread = 0;
      }
   }
  return !(ProcThread || CaptThread);
}
