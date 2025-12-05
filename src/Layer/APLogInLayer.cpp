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
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>



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
        menu_selector(APLogInLayer::onBackButtonClick)
    );

    //back button customization
    auto backButtonLocation = CCMenu::create();
    backButtonLocation->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
    backButtonLocation->addChild(backButton);
    backButtonLocation->setID("top-left-menu");
    this->addChild(backButtonLocation);
    

    //text input port create
    auto txtPort = CCTextInputNode::create(300, 50, "Port", "bigFont.fnt");
    this->inputTxtPort = txtPort;

	//text input port customization
    auto txtPortLocation = CCMenu::create();
    txtPortLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2 - 150, CCDirector::sharedDirector()->getWinSize().height / 2 + 40 });
    txtPortLocation->setID("center-login-port");
    txtPortLocation->addChild(txtPort);
    txtPortLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtPortLocation);


	//text input name create
    auto txtName = CCTextInputNode::create(300, 50, "Name", "bigFont.fnt");
    this->inputTxtName = txtName;

	//text input name customization
    auto txtNameLocation = CCMenu::create();
    txtNameLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2 - 150, CCDirector::sharedDirector()->getWinSize().height / 2 - 40 });
    txtNameLocation->setID("center-login-name");
    txtNameLocation->addChild(txtName);
    txtNameLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtNameLocation);

    
    //connect button sprite
    auto connectButton = ButtonSprite::create(
        "Connect"
    );
	connectButton->setID("center-login-connect-button");
	connectButton->setScale(1.2f);

	//connect button action and position
    auto connectMenuItem = CCMenuItemSpriteExtra::create(
        connectButton,
        this,
        menu_selector(APLogInLayer::onClickConnectButton)
    );
    connectMenuItem->setID("center-login-connect");

    //add to CCMenu to make it clickable
    auto connectMenu = CCMenu::create(connectMenuItem, nullptr);
    connectMenu->ignoreAnchorPointForPosition(true);
	connectMenu->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2 - 120 });
	connectMenu->setID("center-login-connect-menu");
    this->addChild(connectMenu);
    

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

void APLogInLayer::onBackButtonClick(CCObject* btn) {
	auto director = CCDirector::sharedDirector();
    director->popSceneWithTransition(
        .5f, PopTransition::kPopTransitionFade
    );
}

void APLogInLayer::onClickConnectButton(CCObject* btn) {
    
    std::string port = this->inputTxtPort ? this->inputTxtPort->getString() : std::string{};
    std::string name = this->inputTxtName ? this->inputTxtName->getString() : std::string{};

    auto msg = fmt::format("Port: {}\nName: {}", port, name);

    FLAlertLayer::create(
        "Connecting",
        msg.c_str(),
        "OK"
    )->show();
}