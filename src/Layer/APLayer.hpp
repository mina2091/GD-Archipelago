#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class APLayer : public CCLayer {
public:
    // Erzeugt eine Instanz mit dem üblichen Cocos2d-x/Geode-Makro
    CREATE_FUNC(APLayer);

    // Fügt dieses Layer einer neuen Szene hinzu und öffnet sie
    void show();

protected:
    bool init() override;
};