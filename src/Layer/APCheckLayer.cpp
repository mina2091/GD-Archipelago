#include "APCheckLayer.hpp"

/**
 * Shows the Layer
 * @param msg message for Label
 */
void APCheckLayer::show(std::string const& msg) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return;

    auto layer = APCheckLayer::create(msg);
    scene->addChild(layer, 999);
}

/**
 *  creates a bew instance of check layer
 * @param msg message for label
 * @return new APCheckLayer
 */
APCheckLayer* APCheckLayer::create(std::string const& msg) {
    auto ret = new APCheckLayer();
    if (ret && ret->init(msg)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

/**
 * Inits the check layer
 * @param msg Message for Label
 * @return if is initialized
 */
bool APCheckLayer::init(std::string const& msg) {
    if (!CCLayer::init())
        return false;

    auto bg = CCScale9Sprite::create("square02_001.png");
    bg->setContentSize({ 220.f, 50.f });
    bg->setOpacity(180);
    bg->setAnchorPoint({ 0.f, 0.f });
    bg->setPosition({ 8.f, 6.f });

    this->addChild(bg);


    auto label = CCLabelBMFont::create(msg.c_str(), "bigFont.fnt");

    label->setScale(0.32f);                 // kleiner
    label->setAlignment(kCCTextAlignmentCenter);
    label->setAnchorPoint({ 0.5f, 0.5f });

    label->setPosition({
        bg->getContentSize().width / 2,
        bg->getContentSize().height / 2
        });

    bg->addChild(label);

    this->runAction(
    CCSequence::create(
        CCDelayTime::create(3.f),
        CCRemoveSelf::create(true),
        nullptr
        )
    );

    return true;
}

