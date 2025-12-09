#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>
#include "APLogInLayer.hpp"
#include <Geode/ui/General.hpp>
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
    
    //title text
    auto titleLogIn = CCLabelBMFont::create("Log in to Archipelago", "bigFont.fnt");

	//title text customization
	auto titleLogInLocation = CCMenu::create();
	titleLogInLocation->addChild(titleLogIn);
	titleLogInLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2 + 120 });
	titleLogInLocation->setID("center-login-title");
	titleLogInLocation->ignoreAnchorPointForPosition(true);

    this->addChild(titleLogInLocation);


    //text input port create
    auto txtPort = CCTextInputNode::create(300, 50, "Port", "gjFont59-uhd.fnt");
    this->inputTxtPort = txtPort;

	//text input port customization
    auto txtPortLocation = CCMenu::create();
    txtPortLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2 + 50 });
    txtPortLocation->setID("center-login-port");
    txtPortLocation->setColor({ 255, 0, 0 });
    txtPortLocation->addChild(txtPort);
    txtPortLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtPortLocation);


	//text input name create
    auto txtName = CCTextInputNode::create(300, 50, "Name", "gjFont59-uhd.fnt");
    this->inputTxtName = txtName;

	//text input name customization
    auto txtNameLocation = CCMenu::create();
    txtNameLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2});
    txtNameLocation->setID("center-login-name");
    txtNameLocation->addChild(txtName);
    txtNameLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtNameLocation);

    //text input password create
    auto txtPassword = CCTextInputNode::create(300, 50, "Passwort", "gjFont59-uhd.fnt");
    this->inputTxtPassword = txtPassword;

    //text input password customization
    auto txtPasswordLocation = CCMenu::create();
    txtPasswordLocation->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2 - 50 });
    txtPasswordLocation->setID("center-login-password");
    txtPasswordLocation->addChild(txtPassword);
    txtPasswordLocation->ignoreAnchorPointForPosition(true);
    this->addChild(txtPasswordLocation);

    
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
	std::string password = this->inputTxtPassword ? this->inputTxtPassword->getString() : std::string{};

    auto msg = fmt::format("Port: {}\nName: {}\nPassword: {}", port, name, password);

    FLAlertLayer::create(
        "Connecting",
        msg.c_str(),
        "OK"
    )->show();
}
/*
void APLogInLayer::connectSuccess() {
    // goto main screen
}
*/