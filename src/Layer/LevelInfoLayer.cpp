#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

//#include "APLayer.cpp"
#include "APLayer.hpp"

using namespace geode::prelude;

class $modify(APLevelInfoLayer, LevelInfoLayer) {
    public:
    void onBack(CCObject* sender) {
        LevelInfoLayer::onBack(sender);

        auto ap = APLayer::get();

        if (!ap) return;

        ap->updateAPCells(0.0f);
    }
};
