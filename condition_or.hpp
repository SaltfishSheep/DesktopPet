#pragma once

#include "trans.h"

namespace trans {
	class ConditionOr : public ICondition {
    public:
    public:
        bool evaluate() override {
            for (auto condition : conditions) {
                if (condition->evaluate()) {
                    return true;
                }
            }
            return false;
        }
        void loadJson(nlohmann::json& conditionData) override {
            auto& subs = conditionData["subs"];
            for (auto& sub : subs)
                conditions.push_back(trans::loadCondition(sub));
        }
        virtual ~ConditionOr() {
            for (auto condition : conditions) {
                delete condition;
            }
        }
    protected:
        std::list<ICondition*> conditions;
    };
    class ConditionOrFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "Or";
        }
        ICondition* newCondition() override {
            return new ConditionOr();
        }
    };
}