#pragma once
#ifndef GD_ARCHIPELAGO_APCONNECTION_H
#define GD_ARCHIPELAGO_APCONNECTION_H

#include <vector>
#include <string>
#include <unordered_map>

struct Level {
    int ap_progress;
    std::string name;
    std::string id;
    std::string difficulty;
    int difficulty_id;
    int stars_amount;
    std::string song_ids;
    int length;
    bool isPlatformer;
    bool isFinished;
    int playerProgress;
};

namespace APConnection {

    extern std::vector<int64_t> LvlToID;
    extern std::unordered_map<int64_t, int64_t> IDtoLvl;

    void setMinDiff(int i);
    void setMaxDiff(int i);
    void setLevelAmount(int i);
    void setGoalAmount(int i);
    void setChecksPerLevel(int i);
    void setStartingLevelAmount(int i);

    int getLevelAmount();

    void clearTable();
    void worldInputInit();                  //init from world

    bool getIsFinished(int ap_id);
    void setIsFinished(int ap_id);

    void clearItemCallback();
    void itemReceivedCallback(int64_t id, bool notify);
    void locationCheckedCallback(int64_t id);
    std::vector<Level> pickRandomLevels(const std::vector<Level>& allLevels);
    std::vector<Level> loadLevels(const std::string& path);
    void saveLevels(const std::vector<Level>& levels, const std::string& path);

	void setLevelProgress(int64_t ap_id, int prog);
	int64_t getProgressFromID(int64_t ap_id);
    void addToCurrentFinishedLevels();
    void checkForGoalAmount();

    void buildIDTable(const std::vector<Level>& levels);

    void initOnConnect();
    bool isInitComplete(); // returns true when initOnConnect finished
    bool isInitTimedOut(); // true if initOnConnect timed out
	void resetAfterTimeout(); // cleanup after timeout so reconnect can be attempted

    //TEMP
    extern std::vector<Level> randomLevels;
};


#endif //GD_ARCHIPELAGO_APCONNECTION_H