#include "ani.h"
#include "impl_pic.hpp"
#include "impl_files.hpp"
#include "impl_shots.hpp"
#include "impl_repeat.hpp"
#include "impl_custom.hpp"
#include "viewport.h"
#include "trans.h"

#include <iostream>
#include <random>

using namespace ani;
using namespace ani::impl;

using namespace std;

std::map<std::string, IAnimationImplFactory*> factories;
std::map<std::string, Animation*> animations;

Animation* animationLastLoaded = nullptr;

std::random_device randomDevice;
std::default_random_engine randomEngine(randomDevice());

static void initFactories() {

	auto iter = factories.begin();
	while (iter != factories.end()) {
		delete (*iter).second;
		iter++;
	}
	factories.clear();

	list<IAnimationImplFactory*> list;
	list.push_back(new ImplSinglePictureFactory());
	list.push_back(new ImplSequentialFilesFactory());
	list.push_back(new ImplSequentialShotsFactory());
	list.push_back(new ImplRepeatFactory());
	list.push_back(new ImplCustomFactory());


	for (auto factory : list)
		factories[factory->implType()] = factory;
}

void ani::init() {
	initFactories();

	for (const auto& entry : std::filesystem::directory_iterator(file::getPath({ config::guideGlobal.petDirPath, "anis" }))) {
		if (!entry.is_regular_file())
			continue;
		std::string pathStr = entry.path().u8string();
		try {
			if (util::endsWith(pathStr, ".anidef.json")) {
				std::string defPath = pathStr.substr(pathStr.find_last_of('\\') + 1);
				std::cout << "Define animations from " << defPath << '.' << std::endl;
				auto json = file::loadJson(entry.path());
				config::guidePet.loadAnimations(json);
			}
		}
		catch (std::exception& e) {
			std::cout << "Error in loading json, file: " << pathStr << std::endl;
		}
	}
}


Animation* ani::getAnimation(std::string aniName) {
	return animations[aniName];
}
Animation* ani::loadAnimation(std::string aniName, nlohmann::json& aniData) {
    if (animations.find(aniName) != animations.end())
		delete animations[aniName];
    auto aniTemp = new Animation(aniName);
	aniTemp->loadJson(aniData);
    animations[aniName] = aniTemp;
    animationLastLoaded = aniTemp;
	return aniTemp;
}
Animation* ani::getAnimationLastLoaded() {
	return animationLastLoaded;
}

IAnimationImpl* ani::getAnimationImpl(nlohmann::json& implData) {
	auto impl = factories[implData["implType"]]->newImpl();
	impl->loadJson(implData);
	return impl;
}

void ani::Animation::start() {
	this->indexFrame = 0;
	this->tickFramePassed = 0;
	if (this->behavior == AnimationBehavior::holdOnMouse) {
		auto position = sf::Mouse::getPosition(viewport::window);
		this->behaviorData = ((long long)position.x) << 32 | position.y;
	}
}
void ani::Animation::finish() {
	this->indexFrame = -1;
}
bool ani::Animation::isPlaying() {
	return indexFrame != -1;
}
void ani::Animation::render(sf::Sprite& sprite) {
		if (indexFrame == -1)
			return;
		IAnimationImpl& impl = *this->getImpl();
		sprite.setTexture(*impl.frame(indexFrame), true);
		if (impl.frameRect(indexFrame) != nullptr)
			sprite.setTextureRect(*impl.frameRect(indexFrame));

		auto rawScale = sprite.getScale();
		auto implScale = impl.frameScale(indexFrame);
		sf::Vector2f newScale = { rawScale.x * implScale.x * (needReversal ? -1 : 1), rawScale.y * implScale.y };
		sprite.setScale(newScale);

		auto newPosition = sprite.getPosition() + impl.frameOffset(indexFrame);
		if (needReversal) {
			auto& rect = sprite.getTextureRect();
            newPosition.x += std::abs(rect.size.x * newScale.x);
		}
		sprite.setPosition(newPosition);
		
		
		viewport::window.draw(sprite);
	}
void ani::Animation::tick() {
	if (!isPlaying())
		return;
	if (behavior == AnimationBehavior::holdOnMouse) {
		auto relativeX = behaviorData >> 32;
		auto relativeY = behaviorData & 0xFFFFFFFF;
		viewport::window.setPosition(sf::Mouse::getPosition() - sf::Vector2i(relativeX, relativeY));
	}
	else if (behavior == AnimationBehavior::follow) {
		auto relativePosition = sf::Mouse::getPosition(viewport::window);
		relativePosition = relativePosition - sf::Vector2i(config::guideGlobal.windowSize[0] * 0.5, config::guideGlobal.windowSize[1] * 0.5);
		auto moveDirection = sf::Vector2i(
			std::min(config::guidePet.walkSpeed, std::max(-config::guidePet.walkSpeed, relativePosition.x)),
			std::min(config::guidePet.walkSpeed, std::max(-config::guidePet.walkSpeed, relativePosition.y))
		);
        viewport::window.setPosition(viewport::window.getPosition() + moveDirection);
		needReversal = facing * relativePosition.x < 0;
	}
	else if (behavior == AnimationBehavior::walk) {
		if (behaviorData == 0)
			behaviorData = std::uniform_int_distribution(-config::guidePet.walkStepMax, config::guidePet.walkStepMax)(randomEngine);
		needReversal = behaviorData * facing < 0;
		if (behaviorData < 0) {
			auto positionNow = viewport::window.getPosition();
			viewport::window.setPosition({ std::max(0, positionNow.x - config::guidePet.walkSpeed), positionNow.y });
			behaviorData++;
		}
		else if (behaviorData > 0) { 
			auto positionNow = viewport::window.getPosition();
			viewport::window.setPosition({ std::min(config::screenWidth - config::guideGlobal.windowSize[0], positionNow.x + config::guidePet.walkSpeed), positionNow.y});
			behaviorData--;
		}
	}

	auto impl = getImpl();
	int frameCount = impl->frameCount();
	bool isFrameChanged = false;
	while (indexFrame < frameCount && this->tickFramePassed >= impl->frameDuration(indexFrame)) {
		isFrameChanged = true;
		this->tickFramePassed = 0;
		this->indexFrame++;
	}
	if (!isFrameChanged)
		this->tickFramePassed++;
	if (indexFrame >= frameCount) {
		if (isLoop)
			indexFrame = 0;
		else if (isHold)
			indexFrame = frameCount - 1;
		else
			finish();
	}

}
void ani::Animation::loadJson(nlohmann::json& aniData) {

	this->isHold = aniData.contains("isHold") ? aniData["isHold"].get<bool>() : false;
	this->isLoop = aniData.contains("isLoop") ? aniData["isLoop"].get<bool>() : false;

	this->impl = ani::getAnimationImpl(aniData["impl"]);

	if (aniData.contains("transitions")) {
        for (auto& transition : aniData["transitions"]) {
            auto trans = trans::loadTransition(transition);
            this->addTransition(trans);
        }
	}

	if (aniData.contains("canTransitionAnytime"))
        this->setCanTransitionAnytime(aniData["canTransitionAnytime"].get<bool>());

	if (aniData.contains("behavior"))
		if (aniData["behavior"] == "follow")
			this->behavior = follow;
		else if (aniData["behavior"] == "holdOnMouse")
			this->behavior = holdOnMouse;
		else if (aniData["behavior"] == "none")
			this->behavior = none;
		else if (aniData["behavior"] == "walk")
			this->behavior = walk;

	if (aniData.contains("facing"))
		if (aniData["facing"] == "right")
			this->facing = right;
		else if (aniData["facing"] == "left")
			this->facing = left;

}
void ani::Animation::onEnter() {
	this->start();
}
void ani::Animation::onStay() {
	
}
void ani::Animation::onExit() {
	this->finish();
}

IAnimationImpl* ani::Animation::getImpl() {
		return this->impl;
	}
const string& ani::Animation::getAniName() {
		return this->aniName;
	}
ani::Animation::~Animation() {
		if (impl != nullptr)
			delete impl;
	}

util::IState* ani::AnimationController::getState(std::string name) {
	return getAnimation(name);
}

void ani::AnimationController::forceTo(ani::Animation* ani) {
	if (((ani::Animation*)this->getCurrentState()) != nullptr)
		((ani::Animation*)this->getCurrentState())->onExit();
	this->currentState = ani;
	if (((ani::Animation*)this->getCurrentState()) != nullptr)
		((ani::Animation*)this->getCurrentState())->onEnter();
}

void ani::AnimationController::render() {
	if (((ani::Animation*)this->getCurrentState()) != nullptr) {
		this->sprite.setPosition(util::convert(config::guidePet.defaultAniOffset));
		this->sprite.setScale(util::convert(config::guidePet.defaultAniScale));
		((ani::Animation*)this->getCurrentState())->render(this->sprite);
	}
}

void ani::AnimationController::tick() {
    if (((ani::Animation*)this->getCurrentState()) != nullptr) {
		((ani::Animation*)this->getCurrentState())->tick();
	}
	this->transition();
}