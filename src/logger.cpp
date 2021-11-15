#include "logger.h"


static bool shouldLog = false;

void SetupLogger(bool shouldLogIn)
{
    shouldLog = shouldLogIn;
}

bool ShouldLog()
{
    return shouldLog;
}