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

extern int ap_percentage_for_check;

auto local_ppc = ap_percentage_for_check;
//auto progress = ap_percentage_for_check;

class $modify (PlayLayer){

	virtual void postUpdate(float p0) {
		PlayLayer::postUpdate(p0);

		int level = -1; //local
		int ap_id = APConnection::IDtoLvl[m_level->m_levelID];
		geode::log::info("Level ap-ID {}", ap_id);
		int lvl_progress = APConnection::getProgressFromID(ap_id);
		geode::log::info("Progress Level {} = {}%", ap_id+1, lvl_progress);

		auto lvl = APConnection::IDtoLvl.find(m_level->m_levelID);
			if (lvl != APConnection::IDtoLvl.end()) {
				level = lvl->second + 1;

			}else {
				geode::log::info("Level ID {} not in AP", m_level->m_levelID);
				return;
			}

		geode::log::info("{}", this->getCurrentPercent());

		if (APConnection::randomLevels[ap_id].playerProgress < this->getCurrentPercentInt())
		{
			APConnection::randomLevels[ap_id].playerProgress = this->getCurrentPercentInt();
			geode::log::info("Set Level->playerProgress to: {}", APConnection::randomLevels[ap_id].playerProgress);
		}

		if (this->getCurrentPercent() > lvl_progress && lvl_progress != 0 && this->isGameplayActive() || (this->m_levelEndAnimationStarted && !APConnection::getIsFinished(ap_id))) {

			//it doesnt work for 100% yet, no idea why
			AchievementNotifier::sharedState()->notifyAchievement(
				"Check Sent!",
				fmt::format("Check has been sent! ({}%)", lvl_progress).c_str(),
				"../img/archi.png"_spr,
				false
			);

			int loc = 0x100 + (level - 1) * 100 + (lvl_progress/5)-1;
			//256 - 0 * 100 + 1/1  = 256

			geode::log::info("Level {}, {}% complete.", level, lvl_progress);
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

			/*
			if (this->getCurrentPercent() == 100) {
				progress = 101;	//out of the loop
			}
			*/



			if (this->m_levelEndAnimationStarted) {
				APConnection::setIsFinished(ap_id);
				APConnection::addToCurrentFinishedLevels();
				APConnection::checkForGoalAmount();
			}

			//TODO: make it work for 100% and add to something like "finished_levels" so it can track and get win condition
			if (lvl_progress < 100) {
				this->PlayLayer::destroyPlayer(m_player1, nullptr);
				return;
			}

			APConnection::setLevelProgress(ap_id, lvl_progress+local_ppc);

		}
	}
};
