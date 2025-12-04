#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>
#include "APLogInLayer.hpp"
#include <geode/ui/General.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>

using namespace geode::prelude;

bool APLogInLayer::init() {
    if (!CCLayer::init())
        return false;
    
    auto bg = createLayerBG();  //add default background
	bg->setColor({ 50, 50, 50});
    bg->setID("background");
    this->addChild(bg);

    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(APLogInLayer::onButtonClick)
    );
    backButton->setID("back-button");
    
	auto topLeftMenu = CCMenu::create();
	topLeftMenu->setPosition({ 30, CCDirector::sharedDirector()->getWinSize().height - 30 });
	topLeftMenu->addChild(backButton);
    //topLeftMenu->setSizeMult(1.2f);
	topLeftMenu->setID("top-left-menu");
    this->addChild(topLeftMenu);

    // Platz für spätere UI-Initialisierung (Buttons, List, ...)

    return true;
}

void APLogInLayer::show() {
    // Neue Szene erstellen und dieses Layer hinzufügen
    auto scene = CCScene::create();
    scene->addChild(this);

    auto director = CCDirector::sharedDirector();
    if (director->getRunningScene()) {
        // Ersetze die aktuelle Szene durch die neue
        director->pushScene(
            CCTransitionFade::create(
            .5f,
            scene)
        );
    }
    else {
        // Falls keine Szene läuft, starte mit der neuen
        director->runWithScene(scene);
    }
}

void APLogInLayer::onButtonClick(CCObject* btn) {
	auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}