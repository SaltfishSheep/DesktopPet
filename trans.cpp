#include <map>

#include "file.h"
#include "util.h"
#include "trans.h"
#include "config.h"
#include "ani.h"
#include "condition_and.hpp"
#include "condition_or.hpp"
#include "condition_not.hpp"
#include "condition_mouse.hpp"
#include "condition_random.hpp"
#include "condition_false.hpp"
#include "condition_true.hpp"
#include "condition_playend.hpp"

#include <iostream>

using namespace trans;

std::map<std::string, trans::IConditionFactory*> factories;

static void initFactories() {
	std::list<IConditionFactory*> conditionFactories;
    conditionFactories.push_back(new ConditionAndFactory());
    conditionFactories.push_back(new ConditionOrFactory());
    conditionFactories.push_back(new ConditionNotFactory());
    conditionFactories.push_back(new ConditionMouseFactory());
    conditionFactories.push_back(new ConditionRandomFactory());
    conditionFactories.push_back(new ConditionFalseFactory());
    conditionFactories.push_back(new ConditionTrueFactory());
    conditionFactories.push_back(new ConditionPlayEndFactory());
    for (auto factory : conditionFactories) {
        factories[factory->type()] = factory;
    }
}

void trans::init(){
    initFactories();
    for (const auto& entry : std::filesystem::directory_iterator(file::getPath({ config::guideGlobal.petDirPath, "anis" }))) {
        if (!entry.is_regular_file())
            continue;
        std::string pathStr = entry.path().u8string();
        try {
            if (util::endsWith(pathStr, ".anydef.json")) {
                std::string defPath = pathStr.substr(pathStr.find_last_of('\\') + 1);
                std::cout << "Define transition anytime from " << defPath << '.' << std::endl;
                auto json = file::loadJson(entry.path());
                for (auto& transitionData : json.items()) {
                    auto transition = trans::loadTransition(transitionData.value());
                    ani::animationController.addTransitionAnytime(transition);
                    std::cout << "- target: " << transition->getTransition() << std::endl;
                }
            }
        }
        catch (std::exception& e) {
            std::cout << "Error in loading json, file: " << pathStr << std::endl;
        }
    }
}

trans::ICondition* trans::loadCondition(nlohmann::json& conditionData)
{
	auto type = conditionData["type"].get<std::string>();
    //std::cout << factories[type] << std::endl;
    if (factories.find(type) != factories.end()) {
        //std::cout << type << std::endl;
		auto condition = factories[type]->newCondition();
		condition->loadJson(conditionData);
        return condition;
    }
	return nullptr;
}
trans::AnimationTransition* trans::loadTransition(nlohmann::json& transitionData) {
	std::string target = transitionData["target"];
	auto transition = new AnimationTransition(target);
    transition->loadJson(transitionData);
	return transition;
}

bool trans::AnimationTransition::canTransition() {
    if (conditions.size() == 0)
        return true;
    for (auto condition : conditions) {
        //std::cout << this->target << std::endl;
        if (!(condition->evaluate()))
            return false;
    }
    return true;
}

void trans::AnimationTransition::loadJson(nlohmann::json& transitionData) {
    for (auto& conditionData : transitionData["conditions"]) {
        auto condition = loadCondition(conditionData);
        if (condition != nullptr)
            conditions.push_back(condition);
    }
}