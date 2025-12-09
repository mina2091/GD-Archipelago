#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class APLogInLayer : public CCLayer {
public:
    // Erzeugt eine Instanz mit dem üblichen Cocos2d-x/Geode-Makro
    CREATE_FUNC(APLogInLayer);

    // Fügt dieses Layer einer neuen Szene hinzu und öffnet sie
    void show();

	void onBackButtonClick(CCObject* btn);

	void onClickConnectButton(CCObject* btn);

	//void connnectSuccess();

protected:
    bool init() override;

    // callbacks
    CCTextInputNode* inputTxtPort = nullptr;
    CCTextInputNode* inputTxtName = nullptr;
	CCTextInputNode* inputTxtPassword = nullptr;
};