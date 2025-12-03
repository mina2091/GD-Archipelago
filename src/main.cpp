#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>

using namespace geode::prelude;

class $modify(menu, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init())
			return false;
		
		auto bottomLeftMenu = this->getChildByID("bottom-left-menu");
		auto apButton = CCMenuItemSpriteExtra::create(
			//CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			CCSprite::create("archi_shadow.png"_spr),
			this,
			menu_selector(menu::onButtonClick)
		);

		//auto imgTest = CCSprite::create("dsdsdsd.jpg"_spr);
		//auto imgTest2 = CCSprite::create("archi.png"_spr);

		bottomLeftMenu->addChild(apButton);

		apButton->ignoreAnchorPointForPosition(true);


		return true;
	}

	void onButtonClick(CCObject* btn) {
		FLAlertLayer::create("GD Archipelago Online", "pe", "lago")->show();
	}

};