#pragma once

#include "trans.h"

namespace trans { 
    class ConditionAnd : public ICondition {
    public:
        bool evaluate() override {
            for (auto condition : conditions) {
                if (!condition->evaluate()) {
                    return false;
                }
            }
            return true;
        }
        void loadJson(nlohmann::json& conditionData) override {
            auto& subs = conditionData["subs"];
            for (auto& sub : subs)
                conditions.push_back(trans::loadCondition(sub));
        }
        virtual ~ConditionAnd() {
            for (auto condition : conditions) {
                delete condition;
            }
        }
    protected:
        std::list<ICondition*> conditions;
    };
    class ConditionAndFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "And";
        }
        ICondition* newCondition() override {
            return new ConditionAnd();
        }
    };
}
