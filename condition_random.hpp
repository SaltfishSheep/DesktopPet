#pragma once

#include "trans.h"
#include <random>

namespace trans {
    class ConditionRandom : public ICondition {
    public:
        bool evaluate() override {
            return random() < chance;
        }
        void loadJson(nlohmann::json& conditionData) override {
            chance = conditionData["chance"];
        }
    protected:
        float chance = 1.0;
    private:
        static std::random_device randomDevice;
        static std::default_random_engine randomEngine;
        static std::uniform_real_distribution<float> randomDistribution;
        static float random() {
            return randomDistribution(randomEngine);
        }
    };
    std::random_device ConditionRandom::randomDevice;
    std::default_random_engine ConditionRandom::randomEngine(ConditionRandom::randomDevice());
    std::uniform_real_distribution<float> ConditionRandom::randomDistribution(0, 1);

    class ConditionRandomFactory : public IConditionFactory {
    public:
        std::string type() override {
            return "Random";
        }
        ICondition* newCondition() override {
            return new ConditionRandom();
        }
    };
}
