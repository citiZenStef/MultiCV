/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* MoGTLD.h                                                             */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#ifndef MOGTLD_H
#define MOGTLD_H

#include <opencv/cv.h>
#include <opencv2/video/background_segm.hpp>
#include "Vision/AlgoVision.h"
#include "Vision/cvBlob/cvblob.h"

#include "Vision/libopentld/tld/TLD.h"
#include "Vision/libopentld/imacq/ImAcq.h"
#include "Vision/Trajectory.h"

#include "Camera/Structures.h"


#define HEIGHT 20
#define WIDTH  20

using namespace tld;
using namespace cv;
using namespace cvb;
using namespace toolAlgo;
using namespace std;

class MoGTLD : public AlgoVision
{
public:
    MoGTLD();
    ~MoGTLD();

    void execute(Mat in, CvRect* pRoi = 0, bool debug = false);

    struct TLDProcessingSettings TLDSettings;
    struct MOGProcessingSettings MOGSettings;

private:
    Mat Foreground;
    Mat Background;

    CvPoint TargetCenter;

    IplImage Ipl_imgMat;
    IplImage Ipl_foreground;

    BackgroundSubtractorMOG2* MOG;

    bool IsObjectDetected;
    bool IsWindows;

    Point2f BoxPoints[4];
    IplImage* Ipl_foregroundResized;

    TLD *pTLD;
    ImAcq *pImAcq;
    bool ShowOutput;
    const char *PrintResults;
    const char *SaveDir;
    FILE *ResultsFile;
    double Threshold;
    bool ShowForeground;
    bool ShowNotConfident;
    int *InitialBB;
    bool Reinit;
    const char *ModelPath;
    const char *ModelExportFile;
    int Seed;
    CvFont Font;

    int CurrentFrame;
    int Confident;

    Rect* bb;
    CvRect* mogROI;

    Trajectory trajectory;

    bool ReuseFrameOnce;
    bool SkipProcessingOnce;
};

#endif // MOGTLD_H