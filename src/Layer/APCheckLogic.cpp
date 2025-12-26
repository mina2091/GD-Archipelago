#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/cocos/sprite_nodes/CCSprite.h>

#include <Geode/ui/Notification.hpp>
#include <Geode/binding/AchievementNotifier.hpp>
#include <Geode/binding/AchievementManager.hpp>

#include "Archipelago.h"

extern int percentage_for_check;

auto progress = percentage_for_check;

class $modify (PlayLayer){


	virtual void postUpdate(float p0) {
		PlayLayer::postUpdate(p0);

		if (this->getCurrentPercent() > progress) {
			this->PlayLayer::destroyPlayer(m_player1, nullptr);

			AchievementNotifier::sharedState()->notifyAchievement(
				"Check Sent!",
				fmt::format("Check has been sent! ({}%)", progress).c_str(),
				"../img/archi.png"_spr,
				false
			);
			

			progress += percentage_for_check;
		}
	}
};
