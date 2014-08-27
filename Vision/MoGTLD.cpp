/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* MoGTLD.h                                                             */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#include "MoGTLD.h"
#include "Vision/libopentld/tld/TLDUtil.h"

#include <stdio.h>
#include <stdlib.h>

#include <QDir>


//////////////////////////////////////////////////////////////////

MoGTLD::MoGTLD() :
IsObjectDetected(false),
    IsWindows(false)
{
    MOG = new BackgroundSubtractorMOG2();
    pTLD = 0;

    MOGSettings.LearningRate = 0.9;
    MOGSettings.Idx = 0;
    MOGSettings.Count = 0;

    TLDSettings.exportModelAfterRun = false;
    TLDSettings.loadModel = false;
    TLDSettings.showTrajectory = true;
    TLDSettings.trajectoryLength = 10;
    TLDSettings.threshold = 0.5;

    PrintResults = "tld_log.txt";

    TargetCenter.x = 0;
    TargetCenter.y = 0;

    InitialBB = 0;
    Confident = 0;
    CurrentFrame = 0;

    bb = new Rect();
    mogROI = 0;

    Ipl_foregroundResized = cvCreateImage(cvSize(HEIGHT,WIDTH), IPL_DEPTH_8U, 1);

    cvInitFont(&Font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);

    ReuseFrameOnce = false;
    SkipProcessingOnce = false;
}


//////////////////////////////////////////////////////////////////

MoGTLD::~MoGTLD()
{

}


//////////////////////////////////////////////////////////////////

void MoGTLD::execute(Mat in, CvRect *pRoi, bool debug)
{
    // RULEBOOK

    // MoG should always be updated, no matter what
    
    // Detect movements with MoG
    MOG->operator ()(in,Foreground,MOGSettings.LearningRate);

    MOG->getBackgroundImage(Background);

    threshold(Foreground,
              Foreground,
              128,
              255,
              THRESH_BINARY);

    if(!Confident && !in.empty())
    {
        if(mogROI)
        {
            delete mogROI;
            mogROI = 0;
        }

        // Blobs

        // Conversion needed by the old, but efficient, cvblob
        IplImage ipl_foreground = Foreground;

        IplImage* labelImg = cvCreateImage(cvGetSize(&ipl_foreground), IPL_DEPTH_LABEL, 1);

        CvBlobs blobs;
        CvBlob blob;

        // Find blobs
        unsigned int result = cvLabel(&ipl_foreground, labelImg, blobs);

        // Get the biggest blob
        if(blobs.size() > 0)
            blob = *blobs.at(cvLargestBlob(blobs));

        if(blob.area > 25)
        {
            // Create ROI to be used by the TLD algorithm
            mogROI = new CvRect();
            *mogROI = cvRect(blob.minx, blob.miny, blob.maxx - blob.minx, blob.maxy - blob.miny);
        }       
    }

    // TLD
    if( !in.empty() 
        && mogROI 
        && ( mogROI->height > 5 && mogROI->width > 5 )
        && ( mogROI->x >= 0 && mogROI->y >= 0 ) )
    {
        CurrentFrame++;

        Mat grey(in.rows, in.cols, CV_8UC1);
        cvtColor(in, grey, CV_BGR2GRAY);

        if(!pTLD)
        {
            pTLD = new TLD();

            pTLD->detectorCascade->imgWidth     = grey.cols;
            pTLD->detectorCascade->imgHeight    = grey.rows;
            pTLD->detectorCascade->imgWidthStep = grey.step;
        }

        if(!InitialBB)
        {
            if(TLDSettings.showTrajectory)
            {
                trajectory.init(TLDSettings.trajectoryLength);
            }

            InitialBB = new int[4];

            InitialBB[0] = mogROI->x;
            InitialBB[1] = mogROI->y;
            InitialBB[2] = mogROI->width;
            InitialBB[3] = mogROI->height;

            if(TLDSettings.loadModel && !ReuseFrameOnce)
            {
                QString path = QDir(QDir::currentPath()).filePath("model");
                pTLD->readFromFile(path.toStdString().c_str());
                ReuseFrameOnce = true;
            }
            else
            {
                *bb = tldArrayToRect(InitialBB);

                cout << "Starting at " << bb->x << "," << bb->y << "," << bb->width << "," << bb->height << endl;

                pTLD->selectObject(grey, bb);
                SkipProcessingOnce = true;
                ReuseFrameOnce = true;
            }
        }
        else if( mogROI->x != bb->x ||
                 mogROI->y != bb->y ||
                 mogROI->width != bb->width ||
                 mogROI->height != bb->height )
        {
            cout << "Starting at " << mogROI->x << "," << mogROI->y << "," << mogROI->width << "," << mogROI->height << endl;

            bb->x = mogROI->x;
            bb->y = mogROI->y;
            bb->width = mogROI->width;
            bb->height = mogROI->height;

            if(TLDSettings.showTrajectory)
            {
                trajectory.init(TLDSettings.trajectoryLength);
            }

            pTLD->selectObject(grey,bb);
            SkipProcessingOnce = true;
            ReuseFrameOnce = true;
        }

        if(!SkipProcessingOnce)
        {
            pTLD->processImage(in);
        }
        else
        {
            SkipProcessingOnce = false;
        }

        Confident = (pTLD->currConf >= TLDSettings.threshold) ? 1 : 0;

        if(ShowOutput || SaveDir)
        {
            char string[128];

            char learningString[10] = "";

            if(pTLD->learning)
            {
                strcpy(learningString, "Learning");
            }

            sprintf(string, "#%d,Posterior %.2f; #numwindows:%d, %s", CurrentFrame - 1, pTLD->currConf, pTLD->detectorCascade->numWindows, learningString);

            if(pTLD->currBB)
            {
                Scalar rectangleColor = (Confident) ? toolAlgo::target_color[2] : toolAlgo::target_color[3];
                rectangle(in, pTLD->currBB->tl(), pTLD->currBB->br(), rectangleColor, 8, 8, 0);

                if(TLDSettings.showTrajectory)
                {
                    CvPoint center = cvPoint(pTLD->currBB->x+pTLD->currBB->width/2, pTLD->currBB->y+pTLD->currBB->height/2);
                    line(in, cvPoint(center.x-2, center.y-2), cvPoint(center.x+2, center.y+2), rectangleColor, 2);
                    line(in, cvPoint(center.x-2, center.y+2), cvPoint(center.x+2, center.y-2), rectangleColor, 2);
                    trajectory.addPoint(center, rectangleColor);
                }
            }
            else if(TLDSettings.showTrajectory)
            {
                trajectory.addPoint(cvPoint(-1, -1), cvScalar(-1, -1, -1));
            }

            if(TLDSettings.showTrajectory)
            {
                trajectory.drawTrajectory(&in);
            }

            rectangle(in, cvPoint(0, 0), cvPoint(in.cols, 50), toolAlgo::target_color[4], CV_FILLED, 8, 0);
            putText(in, string, cvPoint(25, 25), 1, 1.0, toolAlgo::target_color[5]);

            if(ShowForeground)
            {

                for(size_t i = 0; i < pTLD->detectorCascade->detectionResult->fgList->size(); i++)
                {
                    Rect r = pTLD->detectorCascade->detectionResult->fgList->at(i);
                    rectangle(in, r.tl(), r.br(), toolAlgo::target_color[5], 1);
                }

            }

            if(!grey.empty() && pTLD->currBB)
            {
                QString path = QDir::currentPath().append(QString("/TLD_imageDB/"));

                QDir dir(path);
                if (!dir.exists())
                {
                    dir.mkdir(path);
                }

                path = QDir::currentPath().append(QString("/TLD_imageDB/").append(QString::number(CurrentFrame - 1)).append(".png"));

                cv::Mat subImg = grey(cv::Rect(pTLD->currBB->x,
                                               pTLD->currBB->y,
                                               pTLD->currBB->width,
                                               pTLD->currBB->height));

                imwrite(path.toStdString(), subImg);
            }
        }

        if(ReuseFrameOnce)
        {
            ReuseFrameOnce = false;
        }
    }
}