#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include <stdio.h>

#include "common/error.h"

#include "xeen.h"

XEEN::XeenEngine::XeenEngine(OSystem* syst) : Engine(syst)
{

}

Common::Error XEEN::XeenEngine::run()
{
    printf("YAY\n");
    return Common::kNoError;
}
