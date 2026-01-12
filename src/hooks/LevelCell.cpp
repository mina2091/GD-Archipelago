#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <regex>

#include "../Utils/APConnection.hpp"

using namespace geode::prelude;

class $modify(APLevelCell, LevelCell) {

    //Creates APLevelCell from GJGameLevel
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        // Only modify Archipelago levels
        if (!level || !level->m_creatorName.starts_with("Level "))
            return;

        if (!m_mainLayer)
            return;

        //if level is locked remove all sprites from cell and add a lock icon
        if (level->m_levelID == 1) {
            m_mainLayer->removeAllChildren();

            auto lockIcon = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
            lockIcon->setPosition({ 180, 34 });
            lockIcon->setAnchorPoint({ 0.5f, 0 });
            lockIcon->setID("lock-icon");
            m_mainLayer->addChild(lockIcon);

            return;
        }

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

        //Creates progressbars
        setProgressBar();

        if (!oldSprite)
            return;

        oldSprite->setVisible(false);

        //Sets difficulty display

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

    //Creates the Progressbars for the AP status showing
    //(Locked content, unlocked content and played content)
    void setProgressBar()
    {

        //creates Progressbars to read of the done or still open percentages of levels
        cocos2d::ccColor3B colorUnlockable = { 20, 20, 20 };
        cocos2d::ccColor3B colorUnlocked = { 70, 70, 70 };
        cocos2d::ccColor3B colorPlayed = { 0, 255, 0 };

        //Unlockable content Progressbar. Never editet. Is the last Layer from the 3 bars and is only there to show the
        //remaining progress which isn't unlocked
        auto progressBarUnlockable = ProgressBar::create();
        progressBarUnlockable->setPosition({
            m_mainLayer->getChildByID("length-label")->getPositionX() + 50, //TODO Make Positioning variable
            m_mainLayer->getChildByID("length-label")->getPositionY() - 10
        });
        progressBarUnlockable->setFillColor(colorUnlockable);
        progressBarUnlockable->updateProgress(100.0f); // Beispielwert
        progressBarUnlockable->setID("ap-progress-bar-unlock");
        progressBarUnlockable->setZOrder(0);
        m_mainLayer->addChild(progressBarUnlockable);


        //Unlocked content Progressbar. Shows how far the content of the level is unlocked.
        auto progressBarUnlocked = ProgressBar::create();
        progressBarUnlocked->setPosition({
            m_mainLayer->getChildByID("length-label")->getPositionX() + 50, //TODO Make Positioning variable
            m_mainLayer->getChildByID("length-label")->getPositionY() - 10
            });
        progressBarUnlocked->setFillColor(colorUnlocked);
        progressBarUnlocked->updateProgress(0); // Beispielwert
        progressBarUnlocked->setID("ap-progress-bar-unlocked");
        progressBarUnlocked->setZOrder(1);
        m_mainLayer->addChild(progressBarUnlocked);

        //Played content Progressbar. Shows how much of the content is already successfully played.
        auto progressBarPlayed = ProgressBar::create();
        progressBarPlayed->setPosition({
            m_mainLayer->getChildByID("length-label")->getPositionX() + 50, //TODO Make Positioning variable
            m_mainLayer->getChildByID("length-label")->getPositionY() - 10
            });
        progressBarPlayed->setFillColor(colorPlayed);
        progressBarPlayed->updateProgress(0); // Beispielwert
        progressBarPlayed->setID("ap-progress-bar-played");
        progressBarPlayed->setZOrder(2);
        m_mainLayer->addChild(progressBarPlayed);
    }

    //Sets the state of the played content Progressbar
    void setProgressbarPlayed(float f)
    {
        ProgressBar* progBar = typeinfo_cast<ProgressBar*>(m_mainLayer->getChildByID("ap-progress-bar-played"));

        if (!progBar)
        {
            return;
        }
        progBar->updateProgress(f);
    }

    //Sets the state of the unlocked content Progressbar
    void setProgressbarUnlocked(float f)
    {
        ProgressBar* progBar = typeinfo_cast<ProgressBar*>(m_mainLayer->getChildByID("ap-progress-bar-unlocked"));
        if (!progBar)
        {
            return;
        }
        progBar->updateProgress(f);
    }

    //Gets the right picture for the current difficulty
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
