#include "tick.h"

#include <chrono>
#include <cmath>

#include "viewport.h"
#include "mouse.h"

using namespace tick;
using namespace std::chrono;

std::list<ITickUpdater*> updaters;

double partialTick = 0;
long long passedTicks = 0;

long long lastTime = 0;

void tick::update() {
    auto nowTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    
    partialTick += (nowTime - lastTime) / tick::tickDuration;
    lastTime = nowTime;

    if (partialTick >= 1) {
        partialTick = std::fmod(partialTick, 1);
        passedTicks++;
        tick();
    }
}

void tick::tick() {
    if (forceStop) {
        viewport::window.close();
        return;
    }
    while (auto event = viewport::window.pollEvent())
    {
        if (event->is<sf::Event::Closed>()) {
            viewport::window.close();
            return;
        }
        mouse::handleEvent(*event);
    }
    for (auto updater : updaters)
        updater->tick();
}

long tick::getTicks() {
    return passedTicks;
}

double tick::getPartialTick() {
    return partialTick;
}

void tick::addTickUpdater(ITickUpdater* updater) {
    for (auto updaterIn  : updaters)
        if ((*updaterIn) == (*updater))
            return;
    updaters.push_back(updater);
}

bool tick::ITickUpdater::operator==(const ITickUpdater& other)
{
    return ((ITickUpdater*)this) == (&other);
}
