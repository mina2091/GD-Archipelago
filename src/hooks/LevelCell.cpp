#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/binding/GJGameLevel.hpp>

using namespace geode::prelude;

class $modify(APLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        // Only modify Archipelago levels
        if (!level || level->m_creatorName != "Archipelago")
            return;

        if (!m_mainLayer)
            return;

        auto difficultyContainer = m_mainLayer->getChildByID("difficulty-container");
        if (!difficultyContainer)
            return;


        // Remove old AP sprite if cell reused
        difficultyContainer->removeChildByID("ap-difficulty-sprite");

        auto oldSprite = dynamic_cast<cocos2d::CCSprite*>(
            difficultyContainer->getChildByID("difficulty-sprite")
        );

        if (!oldSprite)
            return;

        oldSprite->setVisible(false);

        const char* frame = getDifficultyFrame(level);
        if (!frame)
            return;

        auto newSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame);
        if (!newSprite)
            return;

        newSprite->setPosition(oldSprite->getPosition());
        newSprite->setZOrder(oldSprite->getZOrder());
        newSprite->setScale(oldSprite->getScale());
        newSprite->setID("ap-difficulty-sprite");

        difficultyContainer->addChild(newSprite);
    }

    static const char* getDifficultyFrame(GJGameLevel* level) {
        if (level->m_demon)
        {
            switch (level->m_difficulty) {
                case GJDifficulty::DemonEasy: return "difficulty_07_btn_001.png";
                case GJDifficulty::DemonMedium: return "difficulty_08_btn_001.png";
                case GJDifficulty::Demon: return "difficulty_06_btn_001.png";
                case GJDifficulty::DemonInsane: return "difficulty_09_btn_001.png";
                case GJDifficulty::DemonExtreme: return "difficulty_10_btn_001.png";
            default: break;
            }
        }
        if (level->m_autoLevel)
            return "difficulty_auto_btn_001.png";

        switch (level->m_difficulty) {
            case GJDifficulty::Easy:
                return "difficulty_01_btn_001.png";
            case GJDifficulty::Normal:
                return "difficulty_02_btn_001.png";
            case GJDifficulty::Hard:
                return "difficulty_03_btn_001.png";
            case GJDifficulty::Harder:
                return "difficulty_04_btn_001.png";
            case GJDifficulty::Insane:
                return "difficulty_05_btn_001.png";
            default:
                return nullptr;
        }
    }
};
