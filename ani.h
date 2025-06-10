#pragma once

#include <json.hpp>
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>

#include "viewport.h"
#include "tick.h"
#include "util.h"

namespace ani {

	void init();

	class IAnimationImpl {
	public:
		virtual int frameCount()=0;
		virtual int frameDuration(int index)=0;
		virtual sf::Vector2f frameOffset(int index)=0;
		virtual sf::Vector2f frameScale(int index)=0;
		virtual sf::IntRect* frameRect(int index) { return nullptr; }
		virtual const sf::Texture* const frame(int index)=0;
		virtual void loadJson(nlohmann::json& implData)=0;
	};

	class IAnimationImplFactory {
	public:
		virtual const std::string implType()=0;
		virtual IAnimationImpl* newImpl()=0;
	};

	enum AnimationBehavior {
		none,
		walk,
		follow,
		holdOnMouse
	};

	enum AnimationFace {
		forward = 0,
		right = 1,
		left = -1
	};

	class Animation : public util::IState {
	public:
		virtual void start();
		virtual void finish();
		virtual bool isPlaying();
		//virtual void tick();
		virtual void render(sf::Sprite& sprite);
		virtual void tick();
		virtual IAnimationImpl* getImpl();
		virtual const std::string& getAniName();
		virtual void loadJson(nlohmann::json& aniData);
		void onEnter() override;
		void onStay() override;
		void onExit() override;
		Animation(std::string aniName) : aniName(aniName), isHold(false), isLoop(false) { }
		virtual ~Animation();
	protected:
		int indexFrame = 0;
		int tickFramePassed = 0;

		std::string aniName;
		bool isHold;
		bool isLoop;

		AnimationBehavior behavior = AnimationBehavior::none;
		long long behaviorData  = 0;
		AnimationFace facing = AnimationFace::forward;

		IAnimationImpl* impl = nullptr;

		bool needReversal = false;
	};

	Animation* getAnimation(std::string aniName);
	Animation* loadAnimation(std::string aniName, nlohmann::json& aniData);
	Animation* getAnimationLastLoaded();

	IAnimationImpl* getAnimationImpl(nlohmann::json& implData);

	inline sf::Texture TEX_EMPTY{};

	class AnimationController : public viewport::IRenderer, public tick::ITickUpdater, public util::IStateMachine {
	public:
		AnimationController() : sprite(sf::Sprite(TEX_EMPTY)) { }
		util::IState* getState(std::string name) override;
		virtual void forceTo(Animation* ani);
		void render() override;
		void tick() override;
	protected:
		sf::Sprite sprite;
	};

	inline AnimationController animationController;

}