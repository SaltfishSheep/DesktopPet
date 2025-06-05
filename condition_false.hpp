#pragma once

#include "trans.h"

namespace trans {
    class ConditionFalse : public ICondition {
    public:
        bool evaluate() override {
            return false;
        }
        void loadJson(nlohmann::json& conditionData) override {
        }
    };
    class ConditionFalseFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "False";
        }
        ICondition* newCondition() override {
            return new ConditionFalse();
        }
    };
}
