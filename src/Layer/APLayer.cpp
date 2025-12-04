#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LazySprite.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>

#include "APLayer.hpp"

using namespace geode::prelude;

bool APLayer::init() {
    if (!CCLayer::init())
        return false;

    // Platz für spätere UI-Initialisierung (Buttons, List, ...)

    auto bg = createLayerBG();
	bg->setID("ap-layer-bg");
	bg->setColor({ 136, 73, 172 });
    this->addChild(bg);

	auto topLeftMenu = CCMenu::create();

    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(APLayer::onButtonClick)
    );

    topLeftMenu->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25});
	topLeftMenu->addChild(backButton);
	topLeftMenu->setZOrder(2);
    topLeftMenu->setID("top-left-menu");
    this->addChild(topLeftMenu);

    return true;
}

void APLayer::show() {
    // Neue Szene erstellen und dieses Layer hinzufügen
    auto scene = CCScene::create();
    scene->addChild(this);

    auto director = CCDirector::sharedDirector();
    if (director->getRunningScene()) {
        // Ersetze die aktuelle Szene durch die neue
        director->pushScene(
            CCTransitionFade::create(
                .5f,
                scene
            )
        );
    }
    else {
        // Falls keine Szene läuft, starte mit der neuen
        director->runWithScene(scene);
    }

}

void APLayer::onButtonClick(CCObject* btn) {
    // Zurück zur vorherigen Szene wechseln
    auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}