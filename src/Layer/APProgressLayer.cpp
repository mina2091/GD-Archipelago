#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LazySprite.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>
#include <Geode/ui/ProgressBar.hpp>
#include "APProgressLayer.hpp"
#include <Geode/ui/ListView.hpp>

using namespace geode::prelude;

bool APProgressLayer::init() {
    if (!CCLayer::init())
        return false;

    // Platz für spätere UI-Initialisierung (Buttons, List, ...)

    auto bg = createLayerBG();
	bg->setID("ap-progress-layer-bg");
	bg->setColor({ 229, 169, 255 });
    this->addChild(bg);

	auto topLeftMenu = CCMenu::create();

    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(APProgressLayer::onButtonClick)
    );

    topLeftMenu->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25});
	topLeftMenu->addChild(backButton);
	topLeftMenu->setZOrder(2);
    topLeftMenu->setID("top-left-menu");
    this->addChild(topLeftMenu);


    /*WIP
    auto apLevelList = ListView::create(apLevelListCells, 50.0f, 358.0f, 220.0f);
    apLevelList->setAnchorPoint({ 0.5f, 0.5f });
    apLevelList->setPosition({
        CCDirector::sharedDirector()->getWinSize().width / 3,
        CCDirector::sharedDirector()->getWinSize().height / 3
        });
    apLevelList->setID("ap-Level-List");
    apLevelList->setZOrder(0);
	this->addChild(apLevelList);
    */


	//creates Progressbars to read of the done or still open percentages of levels
    cocos2d::ccColor3B colorUnlockable = { 20, 20, 20 };
    cocos2d::ccColor3B colorUnlocked = { 70, 70, 70 };
    cocos2d::ccColor3B colorPlayed = { 0, 255, 0 };

    auto progressBarUnlockable = ProgressBar::create();
    progressBarUnlockable->setPosition({
        CCDirector::sharedDirector()->getWinSize().width / 3,
        CCDirector::sharedDirector()->getWinSize().height / 3
        });
    progressBarUnlockable->setFillColor(colorUnlockable);
    progressBarUnlockable->updateProgress(100.0f); // Beispielwert
    progressBarUnlockable->setID("ap-progress-bar-unlock");
    this->addChild(progressBarUnlockable);


    auto progressBarUnlocked = ProgressBar::create();
    progressBarUnlocked->setPosition({
        CCDirector::sharedDirector()->getWinSize().width / 3,
        CCDirector::sharedDirector()->getWinSize().height / 3
        });
	progressBarUnlocked->setFillColor(colorUnlocked);
    progressBarUnlocked->updateProgress(80); // Beispielwert
    progressBarUnlocked->setID("ap-progress-bar-unlock");
    this->addChild(progressBarUnlocked);


    auto progressBarPlayed = ProgressBar::create();
    progressBarPlayed->setPosition({
        CCDirector::sharedDirector()->getWinSize().width / 3,
        CCDirector::sharedDirector()->getWinSize().height / 3
        });
    progressBarPlayed->setFillColor(colorPlayed);
    progressBarPlayed->updateProgress(20.0f); // Beispielwert
    progressBarPlayed->setID("ap-progress-bar-unlock");
    this->addChild(progressBarPlayed);

    return true;
}

void APProgressLayer::show() {
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

void APProgressLayer::onButtonClick(CCObject* btn) {
    // Zurück zur vorherigen Szene wechseln
    auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}
