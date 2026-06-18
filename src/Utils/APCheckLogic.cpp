#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/cocos/sprite_nodes/CCSprite.h>

#include <Geode/ui/Notification.hpp>
#include <Geode/binding/AchievementNotifier.hpp>
#include <Geode/binding/AchievementManager.hpp>

#include "Geode/loader/Log.hpp"

#include <string>

#include "APConnection.hpp"
#include "Archipelago.h"
#include "../Layer/APCheckLayer.hpp"

extern int ap_percentage_for_check;

extern bool logged_in;

class $modify (PlayLayer){

	//TODO: fix issue: only works when updating the game by logging in again
	virtual void postUpdate(float p0) {
		PlayLayer::postUpdate(p0);

		if (!logged_in) {
			return;
		}

		/*Initializing different variables*/
		int level = -1; //local
		int ap_id = APConnection::IDtoLvl[m_level->m_levelID];
		int player_progress = APConnection::getCurrentProgressFromID(ap_id);
		int lvl_progress = APConnection::getProgressFromID(ap_id);

		//TODO: remove logs in postUpdate() when testing is over so we don't flood the logs with text
		/*Just returns Checks for every 5 Percent ticks*/
		if (player_progress % 5) //Maybe change to APConnection::getPPC() ?
		{
			geode::log::info("Progress Level {} = {} % --- Player Progress: {} %", ap_id+1, lvl_progress, player_progress);
		}

		//When Payer is at an endPoint for the run in this level
		if (player_progress == lvl_progress || lvl_progress == 0){
			return;
		}

		int nextPlayerCheckPercentage = player_progress + APConnection::getPPC();

		//When level is finished
		if (nextPlayerCheckPercentage > 100){
			return;
		}

		//Gets the Level
		auto lvl = APConnection::IDtoLvl.find(m_level->m_levelID);
			if (lvl != APConnection::IDtoLvl.end()) {
				level = lvl->second + 1;
			}else {
				geode::log::info("Level ID {} not in AP", m_level->m_levelID);
				return;
			}

		geode::log::info("CurrentPercentAsInt: {}", this->getCurrentPercentInt()); //Duplicate to first Percentage output?

		//when reaching a check
		if (this->getCurrentPercentInt() >= nextPlayerCheckPercentage
			&& this->isGameplayActive()
			|| (this->m_levelEndAnimationStarted && !APConnection::getIsFinished(ap_id))
			) {

			//When a checkpoint is reached send this message to UI
			auto msg = fmt::format("Check has been sent!\n\n Level {}, {}%", ap_id+1, nextPlayerCheckPercentage);
			APCheckLayer::show(msg);

			//Calculates a Level ID with ProgressPercent Combi. Explanation a little bit down below
			int loc = 0x100 + (level - 1) * 100 + (lvl_progress/5)-1;
			//256 - 0 * 100 + 1/1  = 256

			//The level Checkpoint checks, which are regularly logged. And send progress to AP
			geode::log::info("Level {}, {}% complete.", level, nextPlayerCheckPercentage);
			geode::log::info("Sending Item ID: {}", loc);
			geode::log::info("Level ID: {}", m_level->m_levelID);
			AP_SendItem(loc);

			/*
				256 = 05% // 1
				257 = 10% 
				258 = 15% 
				259 = 20% 
				260 = 25%
				275 = 100% 

				356 = 05% // 2
				357 = 10%
				358 = 15% 
				[...]
			*/

			AP_SetLocationCheckedCallback([](int64_t loc) {
				geode::log::info("Location checked: {}", loc);
			});

			//If Level ist finished, set level finished and trigger checks
			if (this->m_levelEndAnimationStarted) {
				APConnection::setIsFinished(ap_id);
				APConnection::addToCurrentFinishedLevels();
				APConnection::checkForGoalAmount();
			}

			//If player is playing and reaching max. of current available level percentage
			//Player is being destroyed and game ist paused
			if (nextPlayerCheckPercentage == lvl_progress && lvl_progress < 100) {
				this->PlayLayer::destroyPlayer(m_player1, nullptr);
				this->PlayLayer::pauseGame(false);
				return;
			}

			//Update currentProgress for the Level and sets LevelProgress
			APConnection::setCurrentProgress(ap_id, nextPlayerCheckPercentage);
			APConnection::setLevelProgress(ap_id, lvl_progress + APConnection::getPPC());

		}
	}
};
