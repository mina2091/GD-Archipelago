#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/LazySprite.hpp>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/ui/General.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>
#include <Geode/binding/TextArea.hpp>
#include <fmt/format.h>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/cocos/menu_nodes/CCMenuItem.h>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/binding/CCMenuItemToggler.hpp>

#include <Archipelago.h>
#include "APLogInLayer.hpp"

#include "APConnection.h"
#include "APLayer.hpp"
#include "APProgressLayer.hpp"

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
    auto connectMenu = CCMenu::create(); 
    connectMenu->addChild(connectMenuItem); 
    connectMenu->ignoreAnchorPointForPosition(true);
    connectMenu->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2 - 120 });
    connectMenu->setID("center-login-connect-menu");
    this->addChild(connectMenu);


    //host toggler buttons
    auto hostSelector = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(APLogInLayer::onClickArchiHostButton),
        1.0f
	);
	hostSelector->setID("host-toggler");

	//host toggler customization
	auto hostSelectorLocation = CCMenu::create();
    hostSelectorLocation->setPosition(25, 25);
    hostSelectorLocation->addChild(hostSelector);
    hostSelectorLocation->setID("center-login-host-toggler");
	this->addChild(hostSelectorLocation);


    //host toggler text
    auto hostText = CCLabelBMFont::create("localhost", "bigFont.fnt");

	//host toggler text customization
    auto hostTextLocation = CCMenu::create();
    hostTextLocation->addChild(hostText);
    hostTextLocation->setPosition(-90, -70);
	hostTextLocation->setScale(0.4f);
	hostTextLocation->setID("center-login-host-toggler-text");
    hostTextLocation->ignoreAnchorPointForPosition(true);

    this->addChild(hostTextLocation);


    //TEMP

    auto apLayerBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(APLogInLayer::tempAPLayerClick)
    );
    auto tmp1 = CCMenu::create();
    tmp1->setPosition({ 75, CCDirector::sharedDirector()->getWinSize().height - 125 });
    tmp1->addChild(apLayerBtn);
	tmp1->setID("top-left-temp-ap-layer-btn");
    this->addChild(tmp1);

    auto progLayerBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"),
        this,
        menu_selector(APLogInLayer::tempProgressLayerClick)
    );
    auto tmp2 = CCMenu::create();
    tmp2->setPosition({ 505, CCDirector::sharedDirector()->getWinSize().height - 125 });
    tmp2->addChild(progLayerBtn);
	tmp2->setID("top-left-temp-prog-layer-btn");
    this->addChild(tmp2);
    
	//TEMP

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

    std::string portStr = this->inputTxtPort->getString();
    std::string slotNameStr = this->inputTxtName->getString();
    std::string passwordStr = this->inputTxtPassword->getString();

    if(inputLocalhost) {
        portStr = fmt::format("localhost:{}", portStr);
    } else {
        portStr = fmt::format("archipelago.gg:{}", portStr);
    }

    //AP Init
    const char* host = portStr.c_str();
    const char* gameName = "Geometry Dash";
    const char* slotName = slotNameStr.c_str();
    const char* password = passwordStr.c_str();

    AP_Init(host, gameName, slotName, password);
    AP_SetItemClearCallback(&APConnection::clearItemCallback);
    AP_SetItemRecvCallback(&APConnection::itemReceivedCallback);
    AP_SetLocationCheckedCallback(&APConnection::locationCheckedCallback);
    AP_Start();


    //auto msg = fmt::format("Port: {}\nName: {}\nPassword: {}", port, slotName, password);

    /*FLAlertLayer::create(
        "Connecting",
        msg.c_str(),
        "OK"
    )->show();*/
}

void APLogInLayer::onClickArchiHostButton(CCObject* btn) {
    auto toggler = static_cast<CCMenuItemToggler*>(btn);
    if (!toggler->isToggled()) {
        inputLocalhost = true;
    } else {
        inputLocalhost = false;
    }
}

/*
void APLogInLayer::connectSuccess() {
    // goto main screen
}
*/

void APLogInLayer::tempProgressLayerClick(CCObject* btn) {
    APProgressLayer::create()->show();
}

void APLogInLayer::tempAPLayerClick(CCObject* btn) {
    APLayer::create()->show();
}