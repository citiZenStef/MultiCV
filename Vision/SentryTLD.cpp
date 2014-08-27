/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* SentryTLD.cpp                                                        */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#include "SentryTLD.h"
#include "Vision/libopentld/tld/TLDUtil.h"

#include <stdio.h>
#include <stdlib.h>

#include <QDir>

SentryTLD::SentryTLD()
{
    pTLD = 0;
    showOutput = 1;
    printResults = "tld_log.txt";
    saveDir = "./img_log/";
    showForeground = 1;
    resultsFile = NULL;

    Settings.exportModelAfterRun = false;
    Settings.loadModel = true;
    Settings.showTrajectory = true;
    Settings.trajectoryLength = 10;
    Settings.threshold = 0.5;

    currentFrame = 0;

    if(Settings.showTrajectory)
    {
        trajectory.init(Settings.trajectoryLength);
    }

    initialBB = 0;
    showNotConfident = true;

    reinit = 0;

    bb = new Rect();

    modelExportFile = "model";
    seed = 0;

    modelPath = ".";
    imAcq = 0;

    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);

    reuseFrameOnce = false;
    skipProcessingOnce = false;
}


//////////////////////////////////////////////////////////////////

void SentryTLD::execute(Mat in, CvRect *pRoi, bool debug)
{
    if( !in.empty() && pRoi && ( pRoi->height > 1 && pRoi->width > 1 )
                    && ( pRoi->x > 0 && pRoi->y > 0 ) )
    {
        // convert input image to IplImage (only for drawing purposes: should be modified eventually)
        ipl_imgMat = in;

        currentFrame++;

        Mat grey(in.rows, in.cols, CV_8UC1);
        cvtColor(in, grey, CV_BGR2GRAY);

        if(!pTLD)
        {
            pTLD = new TLD();

            pTLD->detectorCascade->imgWidth = grey.cols;
            pTLD->detectorCascade->imgHeight = grey.rows;
            pTLD->detectorCascade->imgWidthStep = grey.step;
        }

        if(initialBB == 0)
        {
            if(Settings.showTrajectory)
            {
                trajectory.init(Settings.trajectoryLength);
            }

            initialBB = new int[4];

            initialBB[0] = pRoi->x;
            initialBB[1] = pRoi->y;
            initialBB[2] = pRoi->width;
            initialBB[3] = pRoi->height;

            if(Settings.loadModel && !reuseFrameOnce)
            {
                QString path = QDir(QDir::currentPath()).filePath("model");
                pTLD->readFromFile(path.toStdString().c_str());
                reuseFrameOnce = true;
            }
            else
            {
                *bb = tldArrayToRect(initialBB);

                cout << "Starting at " << bb->x << "," << bb->y << "," << bb->width << "," << bb->height << endl;

                pTLD->selectObject(grey, bb);
                skipProcessingOnce = true;
                reuseFrameOnce = true;
            }
        }
        else if( pRoi->x != bb->x ||
                 pRoi->y != bb->y ||
                 pRoi->width != bb->width ||
                 pRoi->height != bb->height )
        {
            cout << "Starting at " << pRoi->x << "," << pRoi->y << "," << pRoi->width << "," << pRoi->height << endl;

            bb->x = pRoi->x;
            bb->y = pRoi->y;
            bb->width = pRoi->width;
            bb->height = pRoi->height;

            if(Settings.showTrajectory)
            {
                trajectory.init(Settings.trajectoryLength);
            }

            pTLD->selectObject(grey,bb);
            skipProcessingOnce = true;
            reuseFrameOnce = true;
        }

        if(!skipProcessingOnce)
        {
            pTLD->processImage(in);
        }
        else
        {
            skipProcessingOnce = false;
        }

        if(printResults != 0 && resultsFile == 0)
        {
            resultsFile = fopen(printResults, "w");
            if(!resultsFile)
            {
                fprintf(stderr, "Error: Unable to create results-file \"%s\"\n", printResults);
            }
        }

        if(resultsFile)
        {
            if(pTLD->currBB)
            {
                fprintf(resultsFile,
                        "%d %.2d %.2d %.2d %.2d %f\n",
                        currentFrame - 1,
                        pTLD->currBB->x,
                        pTLD->currBB->y,
                        pTLD->currBB->width,
                        pTLD->currBB->height,
                        pTLD->currConf);
            }
            else
            {
                fprintf(resultsFile, "%d NaN NaN NaN NaN NaN\n", currentFrame - 1);
            }
        }

        int confident = (pTLD->currConf >= threshold) ? 1 : 0;

        if(showOutput || saveDir)
        {
            char string[128];

            char learningString[10] = "";

            if(pTLD->learning)
            {
                strcpy(learningString, "Learning");
            }

            sprintf(string, "#%d,Posterior %.2f; #numwindows:%d, %s", currentFrame - 1, pTLD->currConf, pTLD->detectorCascade->numWindows, learningString);

            if(pTLD->currBB)
            {
                CvScalar rectangleColor = (confident) ? toolAlgo::target_color[2] : toolAlgo::target_color[3];
                cvRectangle(&ipl_imgMat, pTLD->currBB->tl(), pTLD->currBB->br(), rectangleColor, 8, 8, 0);

                if(Settings.showTrajectory)
                {
                    CvPoint center = cvPoint(pTLD->currBB->x+pTLD->currBB->width/2, pTLD->currBB->y+pTLD->currBB->height/2);
                    cvLine(&ipl_imgMat, cvPoint(center.x-2, center.y-2), cvPoint(center.x+2, center.y+2), rectangleColor, 2);
                    cvLine(&ipl_imgMat, cvPoint(center.x-2, center.y+2), cvPoint(center.x+2, center.y-2), rectangleColor, 2);
                    trajectory.addPoint(center, rectangleColor);
                }
            }
            else if(Settings.showTrajectory)
            {
                trajectory.addPoint(cvPoint(-1, -1), cvScalar(-1, -1, -1));
            }

            if(Settings.showTrajectory)
            {
                trajectory.drawTrajectory(&in);
            }

            cvRectangle(&ipl_imgMat, cvPoint(0, 0), cvPoint(ipl_imgMat.width, 50), toolAlgo::target_color[4], CV_FILLED, 8, 0);
            cvPutText(&ipl_imgMat, string, cvPoint(25, 25), &font, toolAlgo::target_color[5]);

            if(showForeground)
            {

                for(size_t i = 0; i < pTLD->detectorCascade->detectionResult->fgList->size(); i++)
                {
                    Rect r = pTLD->detectorCascade->detectionResult->fgList->at(i);
                    cvRectangle(&ipl_imgMat, r.tl(), r.br(), toolAlgo::target_color[5], 1);
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

                path = QDir::currentPath().append(QString("/TLD_imageDB/").append(QString::number(currentFrame - 1)).append(".png"));

                cv::Mat subImg = grey(cv::Rect(pTLD->currBB->x,
                                               pTLD->currBB->y,
                                               pTLD->currBB->width,
                                               pTLD->currBB->height));

                imwrite(path.toStdString(), subImg);
            }
        }

        if(reuseFrameOnce)
        {
            reuseFrameOnce = false;
        }

        in = &ipl_imgMat;
    }
}


//////////////////////////////////////////////////////////////////

SentryTLD::~SentryTLD()
{
    cout << "SentryTLD destructor called" << endl;

    if(pTLD && resultsFile)
    {
        fclose(resultsFile);
    }

    if(pTLD && Settings.exportModelAfterRun)
    {
        pTLD->writeToFile(modelExportFile);
    }

    delete pTLD;
}
