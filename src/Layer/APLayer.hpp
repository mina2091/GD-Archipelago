#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>

#include "../Utils/APConnection.hpp"

using namespace geode::prelude;

class APLayer : public CCLayer {
public:

    static APLayer* get();
    static void set(APLayer * layer);

    void updateAPCells(float f);

    CREATE_FUNC(APLayer);

    // Öffnet die Scene mit diesem Layer
    void show();

    // Button-Handler — muss in der Header-Datei deklariert sein
    void onButtonClickPop(CCObject* btn);

    void onLogOutButtonClick(CCObject* btn);

	void openStatsLayer(CCObject* btn);

    GJGameLevel* createLevelShell(const Level& ap);

private:

    static inline APLayer* s_instance = nullptr;

protected:
    bool init() override;
};