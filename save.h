#pragma once
#include <json.hpp>
#include "tick.h"

namespace save {

	class ISaver {
	public:
		virtual void save(nlohmann::json& map) = 0;
		virtual void load(nlohmann::json& map) = 0;
	};

	void addSaver(ISaver* saver);
	void init();

	class SaverController : public tick::ITickUpdater {
	public:
		void tick() override;
		void save();
	protected:
		long long lastSaveTick = 0;
	};

	inline SaverController saverController;

}
