#pragma once
#ifndef GD_ARCHIPELAGO_APCONNECTION_H
#define GD_ARCHIPELAGO_APCONNECTION_H


struct Level {
    std::string name;
    std::string id;
    std::string difficulty;
};

namespace APConnection {

    void clearItemCallback();
    void itemReceivedCallback(int64_t id, bool notify);
    void locationCheckedCallback(int64_t id);
    std::vector<Level> pickRandomLevels(const std::vector<Level>& allLevels, std::size_t count);
    std::vector<Level> loadLevels(const std::string& path);
    void saveLevels(const std::vector<Level>& levels, const std::string& path);

};


#endif //GD_ARCHIPELAGO_APCONNECTION_H