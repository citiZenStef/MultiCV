#ifndef VISIONUNIT_H
#define VISIONUNIT_H

#include <Camera/SharedImageBuffer.h>
#include <Camera/CaptureThread.h>
#include <Camera/ProcessingThread.h>
#include "Vision/AlgoVision.h"

using namespace cv;

class VisionUnit
{

public:
    VisionUnit( int deviceNumber,
                SharedImageBuffer* shImgbuffer,
                bool useChameleon = false,
                QString videoStreamName = "" );

    VisionUnit( int deviceNumber, QString URL, SharedImageBuffer* shImgbuffer );

    ~VisionUnit();

    bool Initialize();
    bool Terminate();

    const bool CameraConnected() const;
    const ProcessingThread* GetProcessingThread() const;

    void StopCaptureThread();
    void StopProcessingThread();

    AlgoVision *SetVisionAlgorithm( const QString& algorithmName );
    AlgoVision *GetVisionAlgorithm();

    QString GetVisionAlgorithmName();

    void SetVisionAlgorithmSettings();

    void SetVideoFileName( const QString& videoName );
    
private:
    ProcessingThread*  ProcThread;
    CaptureThread*     CaptThread;
    SharedImageBuffer* ShrImageBuffer;

    bool IsCameraConnected;
    bool UseChameleonCamera;

    int DeviceNumber;

    CvRect ROI;

    QString VideoFileName;
    QString IpCameraURL;
    QString AlgoName;
};

#endif // VISIONUNIT_H
