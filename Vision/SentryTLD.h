#ifndef SENTRYTLD_H
#define SENTRYTLD_H

#include "Vision/AlgoVision.h"
#include "Vision/libopentld/tld/TLD.h"
#include "Vision/libopentld/imacq/ImAcq.h"
#include "Vision/Trajectory.h"

#include "Camera/Structures.h"

using namespace tld;
using namespace cv;
using namespace std;

class SentryTLD : public AlgoVision
{
    TLD *pTLD;
    ImAcq *imAcq;
    bool showOutput;
    const char *printResults;
    const char *saveDir;
    FILE *resultsFile;
    double threshold;
    bool showForeground;
    bool showNotConfident;
    int *initialBB;
    bool reinit;
    const char *modelPath;
    const char *modelExportFile;
    int seed;
    CvFont font;

    int currentFrame;

    Rect* bb;

    Trajectory trajectory;

    bool reuseFrameOnce;
    bool skipProcessingOnce;

    IplImage ipl_imgMat;

public:
    SentryTLD();
    ~SentryTLD();
    struct TLDProcessingSettings Settings;
    void execute(Mat in, CvRect* pRoi = 0, bool debug = false);
};

#endif // SENTRYTLD_H
