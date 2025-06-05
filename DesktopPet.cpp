#define STATE_DEBUG

#include "config.h"
#include "viewport.h"
#include "tick.h"
#include "ani.h"

#ifdef STATE_DEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup") // Debug
#include "test.h"
#else
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") // Release
#endif

int main()
{

    config::loadConfig();

    viewport::initWindow(256, 256);

#ifdef STATE_DEBUG
    test();
#endif
    
    while (viewport::update()) {
        tick::update();
    }
}