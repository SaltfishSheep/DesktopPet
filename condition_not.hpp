#pragma once

#include "trans.h"

namespace trans {
    class ConditionNot : public ICondition {
    public:
        bool evaluate() override {
            return !condition->evaluate();
        }
        void loadJson(nlohmann::json& conditionData) override {
            condition = trans::loadCondition(conditionData["sub"]);
        }
        virtual ~ConditionNot() {
            delete condition;
        }
    protected:
        ICondition* condition;
    };
    class ConditionNotFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "Not";
        }
        ICondition* newCondition() override {
            return new ConditionNot();
        }
    };
}
