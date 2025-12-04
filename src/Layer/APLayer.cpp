#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LazySprite.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>
#include <Geode/cocos/cocoa/CCArray.h>


#include "APLayer.hpp"

using namespace geode::prelude;

bool APLayer::init() {
    if (!CCLayer::init())
        return false;

	auto size = CCDirector::sharedDirector()->getWinSize();


    //Add background to new scene
    auto bg = createLayerBG();
	bg->setID("ap-layer-bg");
	bg->setColor({ 136, 73, 172 });
    this->addChild(bg);


    //Top Left menu, mainly used te re-enter latest scene
	auto topLeftMenu = CCMenu::create();

    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(APLayer::onButtonClick)
    );

    topLeftMenu->setPosition({ 25, size.height - 25});
	topLeftMenu->addChild(backButton);
	topLeftMenu->setZOrder(2);
    topLeftMenu->setID("top-left-menu");
    this->addChild(topLeftMenu);

    //List of available Archipelago Levels
    auto listLayer = GJListLayer::create(
        CustomListView::create(
            CCArray::create(),
            BoomListType::Level,
            226.0f,
            356.0f
        ),
        "archipelago level list",
        {172,121,202},
        356.0f,
        226.0f,
        0
    );
	listLayer->setZOrder(2);
	listLayer->setPosition(size/2 - listLayer->getContentSize()/2);
    this->addChild(listLayer);

    return true;
}

void APLayer::show() {
	//Create a new scene and add this layer to it
    auto scene = CCScene::create();
    scene->addChild(this);

    auto director = CCDirector::sharedDirector();
    if (director->getRunningScene()) {
		//Push the new scene with a fade transition if a scene is already running
        director->pushScene(
            CCTransitionFade::create(
                .5f,
                scene
            )
        );
    }
    else {
		//Otherwise just run the scene
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