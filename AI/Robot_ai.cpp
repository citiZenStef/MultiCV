#include <assert.h>
#include "Robot_ai.h"


void Robot_AI::init(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_INIT) // ST_Init
            TRANSITION_MAP_ENTRY(ST_INIT)// ST_Idle
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_Track
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_Shoot
            TRANSITION_MAP_ENTRY(ST_INIT)// ST_fail
    END_TRANSITION_MAP(NULL)
}


void Robot_AI::init_OK(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(ST_IDLE) // ST_Init
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_Idle
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_Track
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_Shoot
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_fail
    END_TRANSITION_MAP(NULL)
}


void Robot_AI::startTrack(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_Init
            TRANSITION_MAP_ENTRY(ST_TRACK)// ST_Idle
            TRANSITION_MAP_ENTRY(ST_TRACK)// ST_Track
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_Shoot
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_fail
    END_TRANSITION_MAP(NULL)
}


void Robot_AI::shoot_OK(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_Init
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_Idle
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_Track
            TRANSITION_MAP_ENTRY(ST_TRACK)// ST_Shoot
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_fail
    END_TRANSITION_MAP(NULL)
}


void Robot_AI::detect_OK(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_Init
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_Idle
            TRANSITION_MAP_ENTRY(ST_SHOOT)// ST_Track
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_Shoot
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_fail
    END_TRANSITION_MAP(NULL)
}


void Robot_AI::stopTrack(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_Init
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_Idle
            TRANSITION_MAP_ENTRY(ST_IDLE)// ST_Track
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_Shoot
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)// ST_fail
    END_TRANSITION_MAP(NULL)
}


void Robot_AI::stop(void)
{
    // Transition vers nouvel etat d'apres l'etat courant
    BEGIN_TRANSITION_MAP
            TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_Init
            TRANSITION_MAP_ENTRY(ST_IDLE)// ST_Idle
            TRANSITION_MAP_ENTRY(ST_IDLE)// ST_Track
            TRANSITION_MAP_ENTRY(ST_IDLE)// ST_Shoot
            TRANSITION_MAP_ENTRY(EVENT_IGNORED)// ST_fail
    END_TRANSITION_MAP(NULL)
}

// Etat initial de la machine a etat, on commence ici
void Robot_AI::ST_init()
{
    cout << "Etat INIT" << endl;
    if(!robotFailed && !isRobotOnline) {

        // demande hardware si tout est OK
        comm = new CommRobot();

        shootDone = false;
        shootCounter = 0;
        // TODO Open vision unit

        // charger filtre vision
        this->filtreVision = new SentryMoG();
        // si tout est OK, evenement transition vers Idle
        if(comm->check()) {
            isRobotOnline = true;
            cout << "robot pret, transition vers etat IDLE" << endl;
            InternalEvent(ST_IDLE);
        }
    }
    else if(isRobotOnline) {
        if(comm->check()) {
            cout << "robot pret, transition vers etat IDLE" << endl;
            InternalEvent(ST_IDLE);
        }
        else {
            robotFailed = true;
            cout << "verifier robot, transition vers etat FAIL" << endl;
            InternalEvent(ST_FAIL);
        }
    }
    else {
        // TODO
    }
}


void Robot_AI::ST_idle()
{
    cout << "Etat IDLE" << endl;
    // en attente evenement externe start/stop track
}


void Robot_AI::ST_track()
{
    cout << "Etat TRACK" << endl;

    if(false) {

    }
    else {
        cout << "RE-INIT, transition to FAIL state" << endl;
        InternalEvent(ST_FAIL);
    }
}


void Robot_AI::ST_shoot()
{
    cout << "Etat SHOOT" << endl;
    // envoyer message shoot
    comm->tir();
    cout << "Shoot done, transition to TRACK state" << endl;
    InternalEvent(ST_TRACK);
}


void Robot_AI::ST_fail()
{
    isRobotOnline = false;
    cout << "state FAIL" << endl;
}


Mat Robot_AI::getFrameFromAI()
{
    if(isRobotOnline)
        return in;
}


bool Robot_AI::getAIState()
{
    return isRobotOnline;
}


bool Robot_AI::getShootState()
{
    return shootDone;
}


void Robot_AI::setShootState(bool logicState)
{
    shootDone = logicState;
}
