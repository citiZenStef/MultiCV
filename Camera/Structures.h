/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* Structures.h                                                         */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/

#ifndef STRUCTURES_H
#define STRUCTURES_H

// Qt
#include <QRect>

struct ImageProcessingSettings{
    int smoothType;
    int smoothParam1;
    int smoothParam2;
    double smoothParam3;
    double smoothParam4;
    int dilateNumberOfIterations;
    int erodeNumberOfIterations;
    int flipCode;
    double cannyThreshold1;
    double cannyThreshold2;
    int cannyApertureSize;
    bool cannyL2gradient;
};

struct TLDProcessingSettings{
    double threshold;
    bool showTrajectory;
    bool loadModel;
    bool exportModelAfterRun;
    int trajectoryLength;
};

struct MOGProcessingSettings{
    double LearningRate;
    int Thresh;
    int I;
    int Idx;
    int Count;
};

struct ImageProcessingFlags{
    bool grayscaleOn;
    bool smoothOn;
    bool dilateOn;
    bool erodeOn;
    bool flipOn;
    bool cannyOn;
};

struct MouseData{
    QRect selectionBox;
    bool leftButtonRelease;
    bool rightButtonRelease;
};

struct ThreadStatisticsData{
    int averageFPS;
    int nFramesProcessed;
};

#endif // STRUCTURES_H
