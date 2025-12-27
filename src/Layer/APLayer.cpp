#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/LazySprite.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>
#include <Geode/cocos/cocoa/CCArray.h>
#include <Geode/binding/GJGameLevel.hpp>


#include "APProgressLayer.hpp"
#include "APLayer.hpp"

using namespace geode::prelude;

extern bool logged_in;

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


	//Build CCArray for CustomListView
	auto levelArray = CCArray::create();

	for (const auto& apLevel : APConnection::randomLevels) {
		levelArray->addObject(
			createLevelShell(apLevel)
		);
	}

    //List of available Archipelago Levels
    auto listLayer = GJListLayer::create(
        CustomListView::create(
            levelArray,
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

	//log out button
	auto logOutButton = CCMenuItemSpriteExtra::create(
		CircleButtonSprite::createWithSprite("log_out.png"_spr, 1.0f, CircleBaseColor::Gray, CircleBaseSize::SmallAlt),
		this,
		menu_selector(APLayer::onLogOutButtonClick)
	);

	//log out button customization
	auto logOutButtonLocation = CCMenu::create();
	logOutButtonLocation->setPosition({ 25, 25 });
	logOutButtonLocation->addChild(logOutButton);
	logOutButtonLocation->setID("log-out-button-buttom-left");
	this->addChild(logOutButtonLocation);

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

void APLayer::onLogOutButtonClick(CCObject* btn) {
	logged_in = false;
	auto director = CCDirector::sharedDirector();
	director->popSceneWithTransition(
		.5f, PopTransition::kPopTransitionFade
	);
}

void APLayer::openStatsLayer(CCObject* btn) {
    APProgressLayer::create()->show();
}

//convert level structs to GJGameLevel shells to show in ListView
GJGameLevel* APLayer::createLevelShell(const Level& ap) {
	int levelID = std::stoi(ap.id);

	// create lightweight shell, no auto strings
	auto level = LevelTools::getLevel(levelID, true);

	// REQUIRED UI FIELDS
	level->m_levelID = levelID;
	level->m_levelName = ap.name;
	level->m_levelType = GJLevelType::SearchResult;
	level->m_creatorName = "Archipelago";
	level->m_isUploaded = true;
	level->m_isVerified = true;

	// FEATURED FLAGS
	level->m_featured = true;
	level->m_isEpic = false;          // set if you want epic glow
	level->m_stars = ap.stars_amount;

	// Difficulty (important for face icon)
	level->m_autoLevel = false;
	level->m_rateFeature = true;
	//Demon difficulty icon
	if (ap.difficulty_id > 5)
	{
		level->m_demon = true;
		level->m_difficulty = GJDifficulty::Demon;
		//Demon difficulty(don't ask why geode does this): Easy = 5, Medium = 3, Hard = 1, Insane = 2, Extreme = 4
		switch (ap.difficulty_id-5)
		{
		case 1:
			level->m_demonDifficulty = 5;
			break;
		case 2:
			level->m_demonDifficulty = 3;
			break;
		case 3:
			level->m_demonDifficulty = 1;
			break;
		case 4:
			level->m_demonDifficulty = 2;
			break;
		case 5:
			level->m_demonDifficulty = 4;
			break;
		}
	}
	else
	{
		level->m_demon = false;
		switch (ap.difficulty_id)
		{
		case 1:
			level->m_difficulty = GJDifficulty::Easy;
			break;
		case 2:
			level->m_difficulty = GJDifficulty::Normal;
			break;
		case 3:
			level->m_difficulty = GJDifficulty::Hard;
			break;
		case 4:
			level->m_difficulty = GJDifficulty::Harder;
			break;
		case 5:
			level->m_difficulty = GJDifficulty::Insane;
			break;
		}

	}


	return level;
}
