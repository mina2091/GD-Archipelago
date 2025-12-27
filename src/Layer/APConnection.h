#pragma once
#ifndef GD_ARCHIPELAGO_APCONNECTION_H
#define GD_ARCHIPELAGO_APCONNECTION_H


struct Level {
    std::string name;
    std::string id;
    std::string difficulty;
    int difficulty_id;
    int stars_amount;
};

namespace APConnection {

    void setMinDiff(int i);
    void setMaxDiff(int i);
    void setLevelAmount(int i);
    void setGoalAmount(int i);
    void setChecksPerLevel(int i);
    void setStartingLevelAmount(int i);

    void worldInputInit();                  //init from world

    void clearItemCallback();
    void itemReceivedCallback(int64_t id, bool notify);
    void locationCheckedCallback(int64_t id);
    std::vector<Level> pickRandomLevels(const std::vector<Level>& allLevels);
    std::vector<Level> loadLevels(const std::string& path);
    void saveLevels(const std::vector<Level>& levels, const std::string& path);

    //TEMP
    extern std::vector<Level> randomLevels;
};


#endif //GD_ARCHIPELAGO_APCONNECTION_H