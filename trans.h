#pragma once

#include <json.hpp>

#include "util.h"

namespace trans {
	
	class ICondition {
	public:
		virtual bool evaluate() = 0;
		virtual void loadJson(nlohmann::json& conditionData) = 0;
	};

	class IConditionFactory {
	public:
		virtual std::string type() = 0;
		virtual ICondition* newCondition() = 0;
	};

	class AnimationTransition : public util::IStateTransition {
	public:
		AnimationTransition(std::string target) : IStateTransition(target) {}
		bool canTransition() override;
		virtual void loadJson(nlohmann::json& transitionData);
		virtual ~AnimationTransition()
		{
			for (auto condition : conditions)
			{
				delete condition;
			}
		}
	protected:
		std::list<ICondition*> conditions;
	};

	
	void init();

	ICondition*  loadCondition(nlohmann::json& conditionData);

	AnimationTransition* loadTransition(nlohmann::json& transitionData);

}