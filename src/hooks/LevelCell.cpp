#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <regex>

using namespace geode::prelude;

class $modify(APLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        // Only modify Archipelago levels
        if (!level || !level->m_creatorName.starts_with("Level "))
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

        //Disable coin sprites
        difficultyContainer->getChildByID("coin-icon-1")->setVisible(false);
        difficultyContainer->getChildByID("coin-icon-2")->setVisible(false);
        difficultyContainer->getChildByID("coin-icon-3")->setVisible(false);

        //Disable downloads, likes, orbs
        m_mainLayer->getChildByID("downloads-icon")->setVisible(false);
        m_mainLayer->getChildByID("likes-icon")->setVisible(false);
        m_mainLayer->getChildByID("orbs-icon")->setVisible(false);
        m_mainLayer->getChildByID("downloads-label")->setVisible(false);
        m_mainLayer->getChildByID("likes-label")->setVisible(false);
        m_mainLayer->getChildByID("orbs-label")->setVisible(false);

        //TODO: Use relative values when moving
        //Move length to the left to leave space for progress bar
        m_mainLayer->getChildByID("length-icon")->setPositionX(m_mainLayer->getChildByID("length-icon")->getPositionX() - 45);
        m_mainLayer->getChildByID("length-label")->setPositionX(m_mainLayer->getChildByID("length-label")->getPositionX() - 45);

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
