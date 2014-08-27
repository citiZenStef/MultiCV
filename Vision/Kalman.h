#ifndef KALMAN_H
#define KALMAN_H

#include <time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>


class Kalman
{
public:
        Kalman(void);
        ~Kalman(void);

	virtual void predictionBegin(CvPoint targetCenter);
	virtual void predictionUpdate(CvPoint targetCenter);
	virtual void predictionReport(CvPoint &pnt);

private:
	CvKalman*   Kal;
	CvRandState rng;

	CvMat*  Etat;
	CvMat*  EtatBruit;
	CvMat*  Mesure;
	CvMat*  MesureBruit;

	bool    IsMesure;
	clock_t TpsLastMesure;
};

#endif
