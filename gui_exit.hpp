#pragma once
#include "gui.h"

namespace gui {
	class GuiExit : public IGui {
		bool canOpen() override {
            return true;
        }
        IGuiImplement* open() override {
            save::saverController.save();
            tick::forceStop = true;
            return nullptr;
        }
	};
}
