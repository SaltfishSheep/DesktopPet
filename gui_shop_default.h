#pragma once
#include "gui.h"

namespace gui {
    class GuiShopRenderer : public IGuiImplement {
    public:
        void render() override;
        void tick() override;
        bool isClosed() override;
        virtual void open();
    protected:
        std::string seleted = "convenience";
    };
    class GuiShopDefault : public IGui { 
    public:
        bool canOpen() override;
        IGuiImplement* open() override;
    protected:
        GuiShopRenderer renderer;
    };
}
