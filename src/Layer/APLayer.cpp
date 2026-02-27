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

#include <json/value.h>

#include "Archipelago.h"

using namespace geode::prelude;

#include "../hooks/LevelCell.cpp"

extern bool logged_in;

//List of the displayed levels as CostumListView to access them
CustomListView* m_listView = nullptr;


APLayer* APLayer::get() {
	return s_instance;
}

void APLayer::set(APLayer* layer) {
	s_instance = layer;
}


//Initializes the overview page
bool APLayer::init() {

    if (!CCLayer::init())
        return false;

	auto size = CCDirector::sharedDirector()->getWinSize();


    //Add background to new scene
    auto bg = createLayerBG();
	bg->setID("ap-layer-bg");
	bg->setColor({ 136, 73, 172 });
    this->addChild(bg);

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
		GJGameLevel* gjObject = createLevelShell(apLevel);
		levelArray->addObject(
			gjObject
		);
	}

    //List of available Archipelago Levels
	auto listView = CustomListView::create(
	levelArray,
	BoomListType::Level,
	226.0f,
	356.0f
);

	m_listView = listView;

	auto listLayer = GJListLayer::create(
		listView,
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

	this->scheduleOnce(
	schedule_selector(APLayer::updateAPCells),
	1.0f
	);

	APLayer::set(this);

    return true;
}


//Updates the APLevelCells
//Used to update the Progressbars
void APLayer::updateAPCells(float f)
{

	if (!m_listView) return;

	auto table = m_listView->m_tableView;
	if (!table) return;

	auto content = table->m_contentLayer;
	if (!content) return;

	for (auto node : CCArrayExt<CCNode*>(content->getChildren())) {

		// 1️ Erst Originalklasse
		auto levelCell = static_cast<APLevelCell*>(node);
		if (!levelCell) continue;

		auto level = levelCell->m_level;
		if (!level) continue;
		Level apLevel = APConnection::randomLevels[0];
		for (Level l : APConnection::randomLevels)
		{
			if (l.id == std::to_string(level->m_levelID.value()))
			{
				apLevel = l;
				break;
			}
		}
		try
		{
			levelCell->setProgressbarPlayed(apLevel.playerProgress);
			levelCell->setProgressbarUnlocked(apLevel.ap_progress);
		}catch (const std::exception& e)
		{
			geode::log::error("APLayer::updateAPCells - Error occured while updating progressbar for level '{}': {}",level->m_levelID, e);
		}
	}
}

//Shows the layer
void APLayer::show(){
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

//Pops the scene with transition
void APLayer::onButtonClickPop(CCObject* btn) {
    // Back to last scene
    auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}

//Logs the Player out and pops the scene
void APLayer::onLogOutButtonClick(CCObject* btn) {
	logged_in = false;
	geode::log::info("Logged out!");
	APConnection::resetData();
	auto director = CCDirector::sharedDirector();
	director->popSceneWithTransition(
		.5f, PopTransition::kPopTransitionFade
	);
}

//Shows the stats layer
void APLayer::openStatsLayer(CCObject* btn) {
    APProgressLayer::create()->show();
}

//convert level structs to GJGameLevel shells to show in ListView
GJGameLevel* APLayer::createLevelShell(const Level& ap) {
	int levelID = std::stoi(ap.id);

	// create lightweight shell, no auto strings
	auto level = LevelTools::getLevel(levelID, true);

	//needs to be set early to ensure only archipelago levelCells are modified
	level->m_creatorName = fmt::format("Level {}", APConnection::IDtoLvl[levelID] + 1);

	//don't get data for levels that aren't unlocked
	if (ap.ap_progress == 0)
	{
		return level;
	}

	// REQUIRED UI FIELDS
	level->m_levelID = levelID;
	level->m_levelName = ap.name;
	level->m_levelType = GJLevelType::SearchResult;
	level->m_isUploaded = true;
	level->m_isVerified = true;

	// FEATURED FLAGS
	level->m_featured = true;
	level->m_isEpic = false;          // set if you want epic glow
	level->m_stars = ap.stars_amount;

	//Determine level length 0 = tiny, 1 = short, 2 = medium, 3 = long, 4 = XL
	level->m_levelLength = ap.length;

	//Helper: extract first contiguous number from a string
	auto extract_first_number = [](const std::string& s) -> int {
		for (size_t i = 0; i < s.size(); ++i) {
			if (std::isdigit(static_cast<unsigned char>(s[i]))) {
				size_t j = i;
				while (j < s.size() && std::isdigit(static_cast<unsigned char>(s[j]))) ++j;
				try {
					return std::stoi(s.substr(i, j - i));
				} catch (...) {
					break;
				}
			}
		}
		geode::log::info("APLayer::createLevelShell: no numeric song id found in '{}', defaulting to 0", s);
		return 0;
	};

	//Assert correct song id(s) audioTrack = official song, songID = custom song
	//TODO: add songs for levels with multiple songs
	if (ap.song_ids.starts_with("Multiple")) {

	} else if (ap.song_ids.starts_with("Official")) {
		int audioTrack = extract_first_number(ap.song_ids);
		level->m_audioTrack = audioTrack - 1;
	} else {
		level->m_songID = std::stoi(ap.song_ids);
	}

	// Difficulty (important for face icon)
	level->m_autoLevel = false;
	level->m_rateFeature = true;
	//Demon difficulty icon
	if (ap.difficulty_id > 5)
	{
		level->m_demon = true;
		//Demon difficulty(don't ask why geode does this): Easy = 5, Medium = 3, Hard = 1, Insane = 2, Extreme = 4
		switch (ap.difficulty_id - 5)
		{
		case 1:
			level->m_demonDifficulty = 5;
			level->m_difficulty = GJDifficulty::DemonEasy;
			break;
		case 2:
			level->m_demonDifficulty = 3;
			level->m_difficulty = GJDifficulty::DemonMedium;
			break;
		case 3:
			level->m_demonDifficulty = 1;
			level->m_difficulty = GJDifficulty::Demon;
			break;
		case 4:
			level->m_demonDifficulty = 2;
			level->m_difficulty = GJDifficulty::DemonInsane;

			//Collerb Map; id 111882388; insane demon -> hard demon

			break;
		case 5:
			level->m_demonDifficulty = 4;
			level->m_difficulty = GJDifficulty::DemonExtreme;
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
