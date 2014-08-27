#ifndef ROBOT_AI_H
#define ROBOT_AI_H

#include <iostream>
#include <opencv/cv.h>

#include "StateMachine.h"
#include "Vision/SentryMoG.h"
#include "Controller/CommRobot.h"

using namespace std;

// Structure contenant les donnees passees dans la machine a etats
struct AiData : public EventData
{
    int x;
    int y;
};

class Robot_AI : public StateMachine
{
public:
    Robot_AI() : StateMachine(ST_MAX_STATES) {isRobotOnline = false; robotFailed = false;}

    // Evenements externes pris par la machine a etats
    void init();
    void init_OK();
    void startTrack();
    void stopTrack();
    void detect_OK();
    void shoot_OK();
    void stop();

    // Recuperer image camera
    Mat getFrameFromAI();
    bool getAIState();
    bool getShootState();
    void setShootState(bool logicState);

private:
    // Communication avec le hardware
    CommRobot* comm;
    // Algo de suivi de cible
    SentryMoG* filtreVision;
    // Booleen robot on/off line
    bool isRobotOnline;
    bool robotFailed;
    bool shootDone;
    // Image OpenCV
    Mat in;
    // Centre cible
    CvPoint targetPos;
    int shootCounter;
    // Fonctions d'etat
    void ST_init();
    void ST_idle();
    void ST_track();
    void ST_shoot();
    void ST_fail();

    // Carte d'etats
    BEGIN_STATE_MAP
        STATE_MAP_ENTRY(&Robot_AI::ST_init)
        STATE_MAP_ENTRY(&Robot_AI::ST_idle)
        STATE_MAP_ENTRY(&Robot_AI::ST_track)
        STATE_MAP_ENTRY(&Robot_AI::ST_shoot)
        STATE_MAP_ENTRY(&Robot_AI::ST_fail)
    END_STATE_MAP

    // Enumeration des etats
    enum E_States {
        ST_INIT = 0,
        ST_IDLE,
        ST_TRACK,
        ST_SHOOT,
        ST_FAIL,
        ST_MAX_STATES
    };
};

#endif // ROBOT_AI_H
