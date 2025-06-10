#define STATE_DEBUG

#include "config.h"
#include "viewport.h"
#include "tick.h"
#include "ani.h"
#include "gui.h"
#include "save.h"
#include "goods.h"

#ifdef STATE_DEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup") // Debug
#include "test.h"
#else
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") // Release
#endif

int main()
{
    SetConsoleOutputCP(65001);

    config::loadConfig();

    viewport::initWindow(config::guideGlobal.windowSize[0], config::guideGlobal.windowSize[1]);
    gui::init();
    save::init();
    shop::init();

    tick::addTickUpdater(&save::saverController);

    tick::addTickUpdater(&ani::animationController);
    viewport::addRenderer(&ani::animationController);

    tick::addTickUpdater(&gui::guiController);
    viewport::addRenderer(&gui::guiController);

#ifdef STATE_DEBUG
    test();
#endif
    
    while (viewport::update()) {
        tick::update();
    } 
}