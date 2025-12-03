#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/CCTextInputNode.hpp>

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
		geode::createQuickPopup(
			"Title",            // title
			"auto apButton",   // content
			"connect", "cancel",      // buttons
			[](auto, bool btn2) {
				if (!btn2) {
					FLAlertLayer::create("gdoa", "connected", "ok")->show();
				}
				else {
					FLAlertLayer::create("gdoa", "cancelled", "ok")->show();
				}
			}
		);
	}

	


};