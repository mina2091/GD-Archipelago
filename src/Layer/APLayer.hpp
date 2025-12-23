#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>

#include "APConnection.h"

using namespace geode::prelude;

class APLayer : public CCLayer {
public:
    CREATE_FUNC(APLayer);

    // Öffnet die Scene mit diesem Layer
    void show();

    // Button-Handler — muss in der Header-Datei deklariert sein
    void onButtonClickPop(CCObject* btn);

	void openStatsLayer(CCObject* btn);

    GJGameLevel* createLevelShell(const Level& ap);

protected:
    bool init() override;
};