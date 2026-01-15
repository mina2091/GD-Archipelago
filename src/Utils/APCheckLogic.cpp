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

		int level = -1; //local
		int ap_id = APConnection::IDtoLvl[m_level->m_levelID];
		int player_progress = APConnection::getCurrentProgressFromID(ap_id);
		int lvl_progress = APConnection::getProgressFromID(ap_id);
		//TODO: remove logs in postUpdate() when testing is over so we don't flood the logs with text
		geode::log::info("Progress Level {} = {} % --- Player Progress: {} %", ap_id+1, lvl_progress, player_progress);

		if (player_progress == lvl_progress || lvl_progress == 0){
			return;
		}

		int nextPlayerCheckPercentage = player_progress + APConnection::getPPC();

		if (nextPlayerCheckPercentage > 100){
			return;
		}

		auto lvl = APConnection::IDtoLvl.find(m_level->m_levelID);
			if (lvl != APConnection::IDtoLvl.end()) {
				level = lvl->second + 1;

			}else {
				geode::log::info("Level ID {} not in AP", m_level->m_levelID);
				return;
			}

		geode::log::info("{}", this->getCurrentPercentInt());

		//when reaching a check
		if (this->getCurrentPercentInt() >= nextPlayerCheckPercentage
			&& this->isGameplayActive()
			|| (this->m_levelEndAnimationStarted && !APConnection::getIsFinished(ap_id))
			) {

			auto msg = fmt::format("Check has been sent!\n\n Level {}, {}%", ap_id+1, nextPlayerCheckPercentage);
			APCheckLayer::show(msg);


			int loc = 0x100 + (level - 1) * 100 + (lvl_progress/5)-1;
			//256 - 0 * 100 + 1/1  = 256

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

			if (this->m_levelEndAnimationStarted) {
				APConnection::setIsFinished(ap_id);
				APConnection::addToCurrentFinishedLevels();
				APConnection::checkForGoalAmount();
			}

			if (nextPlayerCheckPercentage == lvl_progress && lvl_progress < 100) {
				this->PlayLayer::destroyPlayer(m_player1, nullptr);
				this->PlayLayer::pauseGame(false);
				return;
			}

			APConnection::setCurrentProgress(ap_id, nextPlayerCheckPercentage);
			APConnection::setLevelProgress(ap_id, lvl_progress + APConnection::getPPC());

		}
	}
};
