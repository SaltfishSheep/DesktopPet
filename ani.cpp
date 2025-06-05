#include "ani.h"
#include "impl_pic.hpp"
#include "impl_files.hpp"
#include "impl_shots.hpp"
#include "impl_repeat.hpp"
#include "impl_custom.hpp"
#include "viewport.h"
#include "trans.h"

#include <iostream>

using namespace ani;
using namespace ani::impl;

using namespace std;

std::map<std::string, IAnimationImplFactory*> factories;
std::map<std::string, Animation*> animations;

Animation* animationLastLoaded = nullptr;

void ani::initFactories() {

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
	this->behaviorData = 0;
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
		sprite.setPosition(sprite.getPosition() + impl.frameOffset(indexFrame));
		auto rawScale = sprite.getScale();
		auto implScale = impl.frameScale(indexFrame);
		sprite.setScale({ rawScale.x * implScale.x, rawScale.y * implScale.y });
		//TODO
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

}
void ani::Animation::onEnter() {
	this->start();
}
void ani::Animation::onStay() {
	if (!isPlaying())
		return;
	auto impl = getImpl();
	if (this->tickFramePassed >= impl->frameDuration(indexFrame)) {
		this->tickFramePassed = 0;
		this->indexFrame++;
	}
	else
		this->tickFramePassed++;
	if (indexFrame >= impl->frameCount()) {
		if (isLoop)
			indexFrame = 0;
		else if (isHold)
			indexFrame = impl->frameCount() - 1;
		else
			finish();
	}
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