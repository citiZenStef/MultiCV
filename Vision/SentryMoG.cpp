/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* SentryMoG.cpp                                                   */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#include "SentryMoG.h"

#include <stdio.h>
#include <stdlib.h>

SentryMoG::SentryMoG() :
        learningRate(0.9),
        isObjectDetected(false),
        isWindows(false),
        index(0),
        targetID(0),
        count(0)
{
    mog = new BackgroundSubtractorMOG2();
    targetCenter.x = 0;
    targetCenter.y = 0;
    ipl_foregroundResized = cvCreateImage(cvSize(HEIGHT,WIDTH), IPL_DEPTH_8U, 1);
}


//////////////////////////////////////////////////////////////////

CvPoint SentryMoG::getTargetPos()
{
    return targetCenter;
}


//////////////////////////////////////////////////////////////////

void SentryMoG::execute(Mat in, CvRect *pRoi, bool debug)
{
    Mat kernel(7,3,CV_8U);
    Point2f c1[] = {cvPoint2D32f(0,0),
                    cvPoint2D32f(HEIGHT,0),
                    cvPoint2D32f(HEIGHT,WIDTH),
                    cvPoint2D32f(0,WIDTH)};

    // Detect movement with MoG background subtraction
    mog->operator ()(in,foreground);
    mog->getBackgroundImage(background);

    threshold(foreground, foreground, 128,
                      255, THRESH_BINARY);

    erode (foreground,foreground,Mat());
    dilate(foreground,foreground,kernel);

    // TODO: use cv::Mat ONLY!!
    ipl_imgMat = in;
    ipl_foreground = foreground;

    IplImage* labelImg = cvCreateImage(cvGetSize(&ipl_imgMat), IPL_DEPTH_LABEL, 1);
    IplImage* test     = cvCreateImage(cvSize(ipl_imgMat.width,ipl_imgMat.height),ipl_imgMat.depth,1);
    cvCvtColor(&ipl_imgMat,test,CV_BGR2GRAY);

    CvBlobs blobs;
    // find blobs
    unsigned int result = cvLabel(&ipl_foreground, labelImg, blobs);
    // filter
    cvFilterByArea(blobs, 10, 50000);
    // display blobs if requested
    if(debug)
        cvRenderBlobs(labelImg, blobs, &ipl_imgMat, &ipl_imgMat, CV_BLOB_RENDER_CENTROID);
    // update tracking
    cvUpdateTracks(blobs, tracks, 20., 15, 30);

    if(debug)
        cvRenderTracks(tracks, &ipl_imgMat, &ipl_imgMat, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);

    // if a target was already acquired, check if it is still there and update the Kalman filter
    if( isObjectDetected )
    {
        CvTracks::iterator it = tracks.find(targetID);
        if( !(it == tracks.end()) )
        {
            CvTrack *track = (*it).second;
            targetCenter.x = track->centroid.x;
            targetCenter.y = track->centroid.y;

            // Build image target

            boxPoints[1] = cvPoint2D32f(targetCenter.x - 15, targetCenter.y - 80);
            boxPoints[0] = cvPoint2D32f(targetCenter.x + 15, targetCenter.y - 80);
            boxPoints[2] = cvPoint2D32f(targetCenter.x - 15, targetCenter.y + 80);
            boxPoints[3] = cvPoint2D32f(targetCenter.x + 15, targetCenter.y + 80);

//             Mat perspectiveMat = getPerspectiveTransform(boxPoints,c1);
//             cvWarpPerspective(test, ipl_foregroundResized, mmat);

            CvPoint predictedTargetCenter = targetCenter;

            kalman->predictionReport(predictedTargetCenter);

            std::cout << "target center x = " << targetCenter.x << ", target center y = " << targetCenter.y << ";" << std::endl;

            cvLine(&ipl_imgMat,cvPoint(targetCenter.x,0),cvPoint(targetCenter.x,ipl_imgMat.height),target_color[5],1);
            cvLine(&ipl_imgMat,cvPoint(0,targetCenter.y),cvPoint(ipl_imgMat.width,targetCenter.y),target_color[5],1);

            cvCircle(&ipl_imgMat, predictedTargetCenter, 20, target_color[0]);
        }
        else
        {
            delete kalman;
            kalman = 0;
            isObjectDetected = false;
        }
    }

    if( !isObjectDetected )
    {
        // else, find a new target
        for( CvTracks::iterator it=tracks.begin();it != tracks.end(); it++ )
        {
            CvTrack *track = (*it).second;
            if( track && !isObjectDetected )
            {
                if( track->lifetime > 20 )
                {
                    targetCenter.x = track->centroid.x;
                    targetCenter.y = track->centroid.y;

                    // Build target image

                    boxPoints[1] = cvPoint2D32f(targetCenter.x - 15, targetCenter.y - 80);
                    boxPoints[0] = cvPoint2D32f(targetCenter.x + 15, targetCenter.y - 80);
                    boxPoints[2] = cvPoint2D32f(targetCenter.x - 15, targetCenter.y + 80);
                    boxPoints[3] = cvPoint2D32f(targetCenter.x + 15, targetCenter.y + 80);

                    //mmat = cvGetPerspectiveTransform(boxPoints,c1,mmat);
                    //cvWarpPerspective(test, ipl_foregroundResized, mmat);

                    std::cout << "target center x = " << targetCenter.x << ", target center y = " << targetCenter.y << ";" << std::endl;

                    isObjectDetected = true;
                    targetID = (*it).first;

                    kalman = new Kalman();
                    kalman->predictionBegin(targetCenter);

                    cvLine(&ipl_imgMat,cvPoint(targetCenter.x,0),cvPoint(targetCenter.x,ipl_imgMat.height),target_color[5],1);
                    cvLine(&ipl_imgMat,cvPoint(0,targetCenter.y),cvPoint(ipl_imgMat.width,targetCenter.y),target_color[5],1);
                }
            }
        }
    }
    
    cvReleaseImage(&labelImg);
    cvReleaseImage(&test);
    in = &ipl_imgMat;

    if( isObjectDetected )
    {
        // no to_string() nor itoa() with gcc (!?)
        stringstream ssX;
        ssX << targetCenter.x;
        string sX = ssX.str();

        stringstream ssY;
        ssY << targetCenter.y;
        string sY = ssY.str();

        string sTargCoord = "Target coordinates (pixels) : x = " + sX +", y = " + sY;
        putText( in, sTargCoord, cvPoint(10,15), FONT_HERSHEY_SIMPLEX, 0.4, target_color[5], 1, CV_AA );
    }
    else
    {
        putText( in, "No target found", cvPoint(10,15), FONT_HERSHEY_SIMPLEX, 0.4, target_color[5], 1, CV_AA );
    }
}


//////////////////////////////////////////////////////////////////

bool SentryMoG::getTargetAvail()
{
    return isObjectDetected;
}


//////////////////////////////////////////////////////////////////

Mat SentryMoG::get_targImg()
{
    Mat foreground(ipl_foregroundResized);
    return foreground;
}


//////////////////////////////////////////////////////////////////

SentryMoG::~SentryMoG()
{
    delete mog;
    cvReleaseImage(&ipl_foregroundResized);
}
