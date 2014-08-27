#ifndef FILTRETRACKING_H
#define FILTRETRACKING_H

#include <opencv/cv.h>
#include <opencv2/video/background_segm.hpp>
#include "Vision/AlgoVision.h"
#include "Kalman.h"
#include "cvBlob/cvblob.h"

#define HEIGHT 20
#define WIDTH  20

using namespace cv;
using namespace cvb;
using namespace toolAlgo;
using namespace std;

class SentryMoG : public AlgoVision
{
public:
    SentryMoG();
    ~SentryMoG();
    void execute(Mat in, CvRect* pRoi = 0, bool debug = false);
    CvPoint getTargetPos();
    bool getTargetAvail();
    Mat get_targImg();

private:
    Mat foreground;
    Mat background;

    CvTracks tracks;
    CvID targetID;

    CvPoint targetCenter;

    IplImage ipl_imgMat;
    IplImage ipl_foreground;

    BackgroundSubtractorMOG2* mog;

    bool isObjectDetected;
    bool isWindows;

    Point2f boxPoints[4];
    IplImage* ipl_foregroundResized;

    Kalman* kalman;

    double learningRate;
    int thresh;
    int i;
    int index;
    int count;
};

#endif // FILTRETRACKING_H
