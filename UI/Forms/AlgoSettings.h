#ifndef ALGOSETTINGS_H
#define ALGOSETTINGS_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "Vision/AlgoVision.h"
#else
#include "Vision/AlgoVision.h"
#endif

class AlgoSettings
{
public:
    AlgoSettings();
    virtual void Register(AlgoVision*) = 0;
    virtual void DisplaySettings() = 0;
    virtual void UpdateSettings() = 0;
};

#endif // ALGOSETTINGS_H
