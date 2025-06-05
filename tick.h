#pragma once

#include <list>

namespace tick {

	inline double tickDuration = 20;

	class ITickUpdater {
	public:
		virtual void tick() = 0;
	};

	void update();
	void tick();
	long getTicks();
	double getPartialTick();
	void addTickUpdater(ITickUpdater* updater);
}