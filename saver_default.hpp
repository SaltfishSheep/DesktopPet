#pragma once
#include "save.h"

namespace save {
	class SaverDefault : public ISaver {
    public:
        unsigned int money = 0;
        void load(nlohmann::json& map) override {
            if (map.contains("money"))
                money = map["money"];
        }
        void save(nlohmann::json& map) override {
            map["money"] = money;
        }
        virtual bool cost(unsigned int cost) {
            if (money >= cost) {
                money -= cost;
                return true;
            }
            return false;
        }
        virtual void addMoney(unsigned int value) {
            money += value;
        }
        virtual unsigned int getMoney() {
            return money;
        }
	};

    inline SaverDefault saverDefault;
}