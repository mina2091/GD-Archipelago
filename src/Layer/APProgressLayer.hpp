#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class APProgressLayer : public CCLayer {
public:
    CREATE_FUNC(APProgressLayer);

    // Öffnet die Scene mit diesem Layer
    void show();

    // Button-Handler — muss in der Header-Datei deklariert sein
    void onButtonClick(CCObject* btn);

protected:
    bool init() override;
};