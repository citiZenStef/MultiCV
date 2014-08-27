/************************************************************************/
/* MultiCV software:                                                    */
/* Computer vision                                                      */
/*                                                                      */
/* Kalman.cpp                                                           */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/************************************************************************/
#include "Kalman.h"


Kalman::Kalman(void)
{
    TpsLastMesure = clock();

    int dynam_params  = 4; //dimension du vecteur d'etats
    int mesure_params = 2; //dimension du vecteur des mesures prises sur le systeme

    Kal  = cvCreateKalman(dynam_params, mesure_params);
    Etat = cvCreateMat   (dynam_params, 1, CV_32FC1); //creation du vecteur d'etats

    // random motion!
    cvRandInit    (&rng, 0, 1, -1, CV_RAND_UNI);
    cvRandSetRange(&rng, 0, 1, 0);

    rng.disttype = CV_RAND_NORMAL;
    cvRand(&rng, Etat);

    EtatBruit   = cvCreateMat( dynam_params,  1, CV_32FC1 ); // (w_k) prise en compte du bruit (LG) introduisant une erreur possible
    Mesure      = cvCreateMat( mesure_params, 1, CV_32FC1 ); // two parameters for x,y  (z_k)
    MesureBruit = cvCreateMat( mesure_params, 1, CV_32FC1 ); // two parameters for x,y (v_k) bruit LG

    cvZero(Mesure);

    // F matrix data matrice de transfert
    // F is transition matrix.  It relates how the states interact 
    // For single input fixed velocity the new value 
    // depends on the previous value and velocity- hence 1 0 1 0 
    // on top line. New velocity is independent of previous 
    // value, and only depends on previous velocity- hence 0 1 0 1 on second row 
    const float F[] = { 
            1, 0, .05, 0, // x + dx (i.e. vx) pour dt = 1 , dt time step a adapter au framerate
            0, 1, 0, .05, // y + dy (i.e. vy) pour dt = 1   ici 0.05 pour un fps de 20
            0, 0, 1, 0,   // dx = dx (vx)
            0, 0, 0, 1    // dy = dy (vy)
                       };
    memcpy(Kal->transition_matrix->data.fl,F,sizeof(F));

    cvSetIdentity( Kal->measurement_matrix,    cvRealScalar(1) );    // matrice H
    cvSetIdentity( Kal->process_noise_cov,     cvRealScalar(1e-1) ); // matrice Q
    cvSetIdentity( Kal->measurement_noise_cov, cvRealScalar(1e-3) ); // matrice R
    cvSetIdentity( Kal->error_cov_post,        cvRealScalar(1) );    // matrice P

    // etat initial aleatoire
    cvRand(&rng, Kal->state_post);
}


//////////////////////////////////////////////////////////////////

Kalman::~Kalman(void)
{
    cvReleaseMat(&Etat);
    cvReleaseMat(&EtatBruit);
    cvReleaseMat(&Mesure);
    cvReleaseMat(&MesureBruit);

    cvReleaseKalman(&Kal);
}


//////////////////////////////////////////////////////////////////

// Set the values for the kalman filter on the first step.
void Kalman::predictionBegin(CvPoint targetCenter)
{ 
    Kal->state_post->data.fl[0] = (float)(targetCenter.x);	//center x
    Kal->state_post->data.fl[1] = (float)(targetCenter.y);	//center y
    Kal->state_post->data.fl[2] = (float)0;	                //dx
    Kal->state_post->data.fl[3] = (float)0;	                //dy
} 


//////////////////////////////////////////////////////////////////

void Kalman::predictionUpdate(CvPoint targetCenter)
{ 
    TpsLastMesure = clock();

    Etat->data.fl[0] = (float)(targetCenter.x);	//center x
    Etat->data.fl[1] = (float)(targetCenter.y);	//center y

    IsMesure = true;
} 


//////////////////////////////////////////////////////////////////

void Kalman::predictionReport(CvPoint &pnt)
{ 
    clock_t timeCurrent = clock(); 
    CvPoint predictPt;

    const CvMat* prediction = cvKalmanPredict( Kal, 0 );

    predictPt.x = prediction->data.fl[0];
    predictPt.y = prediction->data.fl[1];
    //m_dWidth = prediction->data.fl[4]; 
    //m_dHeight = prediction->data.fl[5]; 

    // If we have received the real position recently, then use that position to correct 
    // the kalman filter.  If we haven't received that position, then correct the kalman 
    // filter with the predicted position 
//    if (IsMesure) //(timeCurrent - m_timeLastMeasurement < TIME_OUT_LOCATION_UPDATE)	// update with the real position
//    {
//        IsMesure = false;
//    }
    // My kalman filter is running much faster than my measurements are coming in.  So this allows the filter to pick up
    // from the last predicted state and continue in between measurements (helps the tracker look smoother)
//    else	// update with the predicted position
//    {
        Etat->data.fl[0] = pnt.x;
        Etat->data.fl[1] = pnt.y;
//    }

    // generate measurement noise(z_k) 
    cvRandSetRange( &rng, 0, sqrt(Kal->measurement_noise_cov->data.fl[0]), 0 );
    cvRand( &rng, MesureBruit );

    // zk = Hk * xk + vk 
    // measurement = measurement_error_matrix * current_state + measurement_noise 
    cvMatMulAdd( Kal->measurement_matrix, Etat, MesureBruit, Mesure);

    // adjust Kalman filter state 
    cvKalmanCorrect( Kal, Mesure );

    // generate process noise(w_k) 
    cvRandSetRange( &rng, 0, sqrt(Kal->process_noise_cov->data.fl[0]), 0 );
    cvRand( &rng, EtatBruit );

    // xk = F * xk-1 + B * uk + wk 
    // state = transition_matrix * previous_state + control_matrix * control_input + noise 
    cvMatMulAdd( Kal->transition_matrix, Etat, EtatBruit, Etat);

    pnt.x = predictPt.x;
    pnt.y = predictPt.y;
}
