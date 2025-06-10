#pragma once

#include <list>

namespace tick {

	inline bool forceStop = false;

	inline double tickDuration = 20;

	class ITickUpdater {
	public:
		virtual void tick() = 0;
		virtual bool operator==(const ITickUpdater& other);
	};

	void update();
	void tick();
	long getTicks();
	double getPartialTick();
	void addTickUpdater(ITickUpdater* updater);
}