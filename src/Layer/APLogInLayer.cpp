#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>
#include "APLogInLayer.hpp"
#include <geode/ui/General.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>
#include <Geode/binding/TextArea.hpp>
#include <fmt/format.h>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/cocos/menu_nodes/CCMenuItem.h>


using namespace geode::prelude;

bool APLogInLayer::init() {
    if (!CCLayer::init())
        return false;
    
    //add default background
    auto bg = createLayerBG();  
	bg->setColor({ 50, 50, 50});
    bg->setID("background");
    this->addChild(bg);

    //back button
    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(APLogInLayer::onButtonClick)
    );

    //back button customization
    auto backButtonLocation = CCMenu::create();
    backButtonLocation->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
    backButtonLocation->addChild(backButton);
    backButtonLocation->setID("top-left-menu");
    this->addChild(backButtonLocation);
    

    //text input port create
    auto txtPort = CCTextInputNode::create(300, 50, "Port", "bigFont.fnt");

	//text input port customization
    auto txtPortLocation = CCMenu::create();
    txtPortLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2 - 150, CCDirector::sharedDirector()->getWinSize().height / 2 + 40 });
    txtPortLocation->setID("center-login-port");
    txtPortLocation->addChild(txtPort);
    txtPortLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtPortLocation);


	//text input name create
    auto txtName = CCTextInputNode::create(300, 50, "Name", "bigFont.fnt");

	//text input name customization
    auto txtNameLocation = CCMenu::create();
    txtNameLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2 - 150, CCDirector::sharedDirector()->getWinSize().height / 2 - 40 });
    txtNameLocation->setID("center-login-name");
    txtNameLocation->addChild(txtName);
    txtNameLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtNameLocation);


    //connect button sprite
    auto connectButton = ButtonSprite::create(
        "Connect", 200, true, "bigFont.fnt", "GJ_button_01.png", 50, 1.0f
    );

    //connect button customization 
    auto connectButtonLocation = CCMenuItem::create(
        this,
        menu_selector(APLogInLayer::onClickConnectButton) //macht noch nichts funktionales
    );
    connectButtonLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2 - 120 });
    connectButtonLocation->setID("center-login-connect");
    connectButtonLocation->addChild(connectButton);
    connectButtonLocation->ignoreAnchorPointForPosition(true);
    this->addChild(connectButtonLocation);

    // Platz für spätere UI-Initialisierung (Buttons, List, ...)

    return true;
}

void APLogInLayer::show() {
    // Neue Szene erstellen und dieses Layer hinzufügen
    auto scene = CCScene::create();
    scene->addChild(this);

    auto director = CCDirector::sharedDirector();
    if (director->getRunningScene()) {
        // Ersetze die aktuelle Szene durch die neue
        director->pushScene(
            CCTransitionFade::create(
            .5f,
            scene)
        );
    }
    else {
        // Falls keine Szene läuft, starte mit der neuen
        director->runWithScene(scene);
    }
}

void APLogInLayer::onButtonClick(CCObject* btn) {
	auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}

void APLogInLayer::onButtonClick2(CCObject* btn) {
	FLAlertLayer::create(
        "Connect Button Clicked!",
        "OK",
        "You clicked the connect button."
	)->show();
}

void APLogInLayer::onClickConnectButton(CCObject* btn) {
    FLAlertLayer::create(
        "awa",
        "waw",
        "huuuu"
    )->show();
}