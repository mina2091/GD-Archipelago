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
        menu_selector(APLayer::onButtonClickPop)
    );

    topLeftMenu->setPosition({ 25, size.height - 25});
	topLeftMenu->addChild(backButton);
	topLeftMenu->setZOrder(2);
    topLeftMenu->setID("top-left-menu");
    this->addChild(topLeftMenu);


	//Top Right Menu for refreshing the list
	auto topRightMenu = CCMenu::create();

    auto refreshButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        this,
		menu_selector(APLayer::onButtonClickPop)                    //TODO: Change to CustomListView reloadAll or custom AP Level reload function
	);

	topRightMenu->setPosition({ size.width - 25, size.height - 25 });
	topRightMenu->addChild(refreshButton);
	topRightMenu->setZOrder(2);
	topRightMenu->setID("top-right-menu");
	this->addChild(topRightMenu);
    

    //Bottom Right Menu for Stats Scene
	auto bottomRightMenu = CCMenu::create();

    auto statsButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_statsBtn_001.png"),
        this,
        menu_selector(APLayer::openStatsLayer)
	);

	bottomRightMenu->setPosition({ size.width - 25, 25 });
	bottomRightMenu->addChild(statsButton);
	bottomRightMenu->setZOrder(2);
	bottomRightMenu->setID("bottom-right-menu");
	this->addChild(bottomRightMenu);


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

void APLayer::onButtonClickPop(CCObject* btn) {
    // Back to last scene
    auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}

void APLayer::openStatsLayer(CCObject* btn) {
    APProgressLayer::create()->show();
}