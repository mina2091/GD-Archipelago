#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>
#include "Layer/APLogInLayer.hpp"
#include "Layer/APProgressLayer.hpp"
#include "Layer/APLayer.hpp"

using namespace geode::prelude;

bool logged_in = false;

class $modify(menu, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init())
			return false;

		auto bottomLeftMenu = this->getChildByID("bottom-left-menu");
		auto apButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSprite("archi_shadow.png"_spr, 1.2f),
			this,
			menu_selector(menu::onAPButtonClick)
		);

		bottomLeftMenu->addChild(apButton);
		apButton->ignoreAnchorPointForPosition(true);


		return true;
	}

	void onAPButtonClick(CCObject * btn) {
		if(!logged_in)
			APLogInLayer::create()->show();
		else
			APLayer::create()->show();
	}

};