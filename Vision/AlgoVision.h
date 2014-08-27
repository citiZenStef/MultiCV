#ifndef ALGOVISION_H
#define ALGOVISION_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <iostream>


using namespace cv;
using namespace std;

namespace toolAlgo
{
    const CvScalar target_color[6] = {
        {{   0,   0, 255 }}, // Red
        {{   0, 255,   0 }}, // Green
        {{ 255,   0,   0 }}, // Blue
        {{   0, 255, 255 }}, // Yellow
        {{   0,   0,   0 }}, // Black
        {{ 255, 255, 255 }}  // White
    };
}

class AlgoVision
{
public:
    virtual ~AlgoVision(){cout<<"Vision base-class destructor called"<<endl;}
    virtual void execute(Mat img, CvRect* pRoi = 0, bool debug = false) = 0;
   // virtual CvPoint getTargetPos() = 0;
   // virtual bool getTargetAvail() = 0;
   // virtual Mat get_targImg() = 0;
};

#endif // ALGOVISION_H
