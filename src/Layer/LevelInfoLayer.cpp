#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

//#include "APLayer.cpp"
#include "APLayer.hpp"
#include "../Utils/Events.hpp"

using namespace geode::prelude;

class $modify(APLevelInfoLayer, LevelInfoLayer) {
    public:
    void onBack(CCObject* sender) {
        LevelInfoLayer::onBack(sender);
        LevelInfoClosedEvent().post();

        auto ap = APLayer::get();

        if (!ap) return;

        ap->updateAPCells(0.0f);

    }
};
