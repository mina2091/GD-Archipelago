#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/cocos/sprite_nodes/CCSprite.h>

#include <Geode/ui/Notification.hpp>
#include <Geode/binding/AchievementNotifier.hpp>
#include <Geode/binding/AchievementManager.hpp>

#include "Geode/loader/Log.hpp"

#include <string>

#include "Archipelago.h"

extern int ap_percentage_for_check;

auto local_ppc = ap_percentage_for_check;
auto progress = ap_percentage_for_check;

class $modify (PlayLayer){


	virtual void postUpdate(float p0) {
		PlayLayer::postUpdate(p0);

		if (this->getCurrentPercent() > progress) {

			//it doesnt work for 100% yet, no idea why
			AchievementNotifier::sharedState()->notifyAchievement(
				"Check Sent!",
				fmt::format("Check has been sent! ({}%)", progress).c_str(),
				"../img/archi.png"_spr,
				false
			);

			int level = 2;

			int loc = 0x100 + (level - 1) * 100 + (progress/5)-1;
			//256 - 0 * 100 + 1/1  = 256

			geode::log::info("Sending Item ID: {}", loc);
			AP_SendItem(loc);

			/*
				256 = 05% // 0
				257 = 10% // 1
				258 = 15% // 2
				259 = 20% // 3
				260 = 25% // 4
			*/

			AP_SetLocationCheckedCallback([](int64_t loc) {
				geode::log::info("Location checked: {}", loc);
			});

			if (this->getCurrentPercent() == 100) {
				progress = 101;	//out of the loop
			}

			if (progress < 100) {
				progress += local_ppc;
				this->PlayLayer::destroyPlayer(m_player1, nullptr);
			}
		}
	}
};
