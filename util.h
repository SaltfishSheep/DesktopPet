#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <map>

namespace util {

	template<class T>
	sf::Vector2<T> convert(std::vector<T> stdVec) {
		return sf::Vector2<T> {stdVec[0], stdVec[1]};
	};

	class IStateTransition {
	public:
		IStateTransition(std::string target) : target(target) {}
		virtual bool canTransition() = 0;
		virtual std::string getTransition() {
			return target;
		}
	protected:
		std::string target;
	};

	class IState {
	public:
		virtual bool canTransitionAnytime() {
			return canTransAnytime;
		}
		virtual void setCanTransitionAnytime(bool flag) {
			canTransAnytime = flag;
		}
		virtual std::string getTransition() {
			for (auto transition : transitions)
				if (transition->canTransition())
					return transition->getTransition();
			return "";
		}
		virtual void addTransition(IStateTransition* transition) {
			transitions.push_back(transition);
		}
		virtual void onEnter() {};
		virtual void onStay() {};
		virtual void onExit() {};
		virtual ~IState() {
			for (auto transition : transitions)
				delete transition;
		}
	protected:
		std::list<IStateTransition*> transitions;
		bool canTransAnytime = true;
	};

	class IStateMachine {
	public:
		virtual void addTransitionAnytime(IStateTransition* transition) {
			transitionsAnytime.push_back(transition);
		}
		virtual IState* getState(std::string name) = 0;
		virtual IState* getCurrentState() {
			return currentState;
		};
		virtual bool transition() {
			if (currentState == nullptr || currentState->canTransitionAnytime()) {
				for (auto transition : transitionsAnytime) {
					if (transition->canTransition()) {
                        currentState->onExit();
                        currentState = getState(transition->getTransition());
                        currentState->onEnter();
						return true;
					}
				}
			}
			auto transition = currentState->getTransition();
			if (!transition.empty()) {
				auto nextState = getState(transition);
				currentState->onExit();
				currentState = nextState;
				currentState->onEnter();
				return true;
			}
			else {
				currentState->onStay();
				return false;
			}
		}
	protected:
		IState* currentState = nullptr;
		std::list<IStateTransition*> transitionsAnytime;
	};

}