#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>

using namespace geode::prelude;

class $modify(menu, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init())
			return false;
		
		auto mainmenubutton = this->getChildByID("bottom-left-menu");
		auto btn = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(menu::onButtonClick)
			
		);

		mainmenubutton->addChild(btn);

		return true;
	}

	void onButtonClick(CCObject* btn) {
		FLAlertLayer::create("GD Archipelago Online", "pe", "lago")->show();
	}

};