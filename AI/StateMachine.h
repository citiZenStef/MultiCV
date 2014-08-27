#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdio.h>
#include "EventData.h"

struct StateStruct;

class StateMachine
{
public:
    StateMachine(int maxStates);
    virtual ~StateMachine() {}
protected:
    enum { EVENT_IGNORED = 0xFE, CANNOT_HAPPEN };
    unsigned char currentState;
    void ExternalEvent(unsigned char, EventData* = NULL);
    void InternalEvent(unsigned char, EventData* = NULL);
    virtual const StateStruct* GetStateMap() = 0;
private:
    const int _maxStates;
    bool _eventGenerated;
    EventData* _pEventData;
    void StateEngine(void);
};

typedef void (StateMachine::*StateFunc)(EventData*);
struct StateStruct
{
    StateFunc pStateFunc;
};

#define BEGIN_STATE_MAP \
public:\
const StateStruct* GetStateMap() {\
    static const StateStruct StateMap[] = {

#define STATE_MAP_ENTRY(entry)\
    { reinterpret_cast<StateFunc>(entry) },

#define END_STATE_MAP \
    { static_cast<StateFunc>(NULL) }\
    }; \
    return &StateMap[0]; }

#define BEGIN_TRANSITION_MAP \
    static const unsigned char TRANSITIONS[] = {\

#define TRANSITION_MAP_ENTRY(entry)\
    entry,

#define END_TRANSITION_MAP(data) \
    0 };\
    ExternalEvent(TRANSITIONS[currentState], data);

#endif // STATEMACHINE_H
