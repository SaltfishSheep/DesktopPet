#pragma once

#include "trans.h"

namespace trans {
    class ConditionTrue : public ICondition {
    public:
        bool evaluate() override {
            return true;
        }
        void loadJson(nlohmann::json& conditionData) override {
        }
    };
    class ConditionTrueFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "True";
        }
        ICondition* newCondition() override {
            return new ConditionTrue();
        }
    };
}
