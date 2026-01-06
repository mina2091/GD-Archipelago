#include "APConnection.hpp"
#include "Archipelago.h"
#include <../../lib/json.hpp>
#include <fstream>
#include <vector>
#include <random>
#include <unordered_map>
#include <numeric>
#include <string>

// for convenience
using json = nlohmann::json;

//data needed from .yaml
//idk if they need to be "= 0" but it works so whatever
auto ap_min_diff = 0;
auto ap_max_diff = 0;
auto ap_level_amount = 100; //test value for now
auto ap_goal_amount = 0;
auto ap_checks_per_level = 0;
auto ap_starting_level_amount = 0;
auto ap_percentage_for_check = 20; //test value for now

namespace APConnection {
    std::vector<int64_t> lvlToID;
    std::unordered_map<int64_t, int64_t> IDtoLvl;
}

void APConnection::clearItemCallback() {
    geode::log::info("APConnection::clearItemCallback");
}

void APConnection::itemReceivedCallback(int64_t id, bool notify) {
    geode::log::info("Received Item ID: {}", id);

    //[GD-Archipelago]: APConnection::itemReceivedCallback, 356 - Level 1
    //[GD-Archipelago]: APConnection::itemReceivedCallback, 456 - Level 2

    id = (id - 56)/100-3;
	// (356 - 56) / 100 - 3 = 0
	// (456 - 56) / 100 - 3 = 1

	auto new_progress = getProgressFromID(id) + ap_percentage_for_check;
	geode::log::info("Updated Level {} progress from {}% to {}%", id + 1, getProgressFromID(id), new_progress);
	setLevelProgress(id, new_progress);
}

void APConnection::locationCheckedCallback(int64_t id) {
    geode::log::info("Received Location Check ID: {}", id);
}

//convert into local variables
void APConnection::setMinDiff(int i){
    ap_min_diff = i;
}

void APConnection::setMaxDiff(int i){
    ap_max_diff = i;
}

void APConnection::setLevelAmount(int i){
    ap_level_amount = i;
}

void APConnection::setGoalAmount(int i)
{
    ap_goal_amount = i;
}

void APConnection::setChecksPerLevel(int i){
    ap_checks_per_level = i;
    ap_percentage_for_check = 100 / i;
}

void APConnection::setStartingLevelAmount(int i){
    ap_starting_level_amount = i;
}

//takes slot_fill_data from world and converts into local variables
void APConnection::worldInputInit(){
    AP_RegisterSlotDataIntCallback("min_diff", &setMinDiff);
    AP_RegisterSlotDataIntCallback("max_diff", &setMaxDiff);
    AP_RegisterSlotDataIntCallback("level_amount", &setLevelAmount);
    AP_RegisterSlotDataIntCallback("goal_amount", &setGoalAmount);
    AP_RegisterSlotDataIntCallback("checks_per_level", &setChecksPerLevel);
    AP_RegisterSlotDataIntCallback("starting_level_amount", &setStartingLevelAmount);
}

std::vector<Level> APConnection::pickRandomLevels(const std::vector<Level>& allLevels) {

    if (ap_level_amount > allLevels.size()) {
        throw std::runtime_error("Requested more levels than available");
    }

    std::vector<Level> result;
    result.reserve(ap_level_amount);

    std::vector<std::size_t> indices(allLevels.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(indices.begin(), indices.end(), rng);

    /*for (std::size_t i = 0; i < ap_level_amount; ++i) {
        result.push_back(allLevels[indices[i]]);
    }*/

    //Only add non-platformer levels
    std::size_t i = 0;
    std::size_t validAmount = 0;
    while (validAmount < ap_level_amount) {
        if (!allLevels[indices[i]].isPlatformer
            && allLevels[indices[i]].difficulty_id <= 1 //TODO: Nach dem Debuggen entfernen, weil Schwierigkeit permanent Einfach ist.
            ) {
            result.push_back(allLevels[indices[i]]);
            validAmount++;
        }
        i++;
    }

    APConnection::buildIDTable(result);

    return result;
}

//TODO: fix order of name, id and entry (currently reversed)
std::vector<Level> APConnection::loadLevels(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open levels.json");
    }

    json j;
    file >> j;

    if (!j.is_array()) {
        throw std::runtime_error("levels.json: expected top-level array");
    }

    std::vector<Level> levels;
    levels.reserve(j.size());

    for (const auto& entry : j) {
        // Safe reads with sensible defaults
        std::string name = "";
        std::string id = "";
        std::string difficulty = "";
        int difficulty_id = 0;
        int stars_amount = 0;
        std::string song_ids = "";
        int length = 0;
        bool isPlatformer = false;

        try {
            if (entry.contains("name") && !entry["name"].is_null() && entry["name"].is_string()) {
                name = entry["name"].get<std::string>();
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'name' field, using empty string: {}", e.what());
        }

        try {
            if (entry.contains("id") && !entry["id"].is_null() && entry["id"].is_string()) {
                id = entry["id"].get<std::string>();
            } else if (entry.contains("id") && !entry["id"].is_null() && entry["id"].is_number_integer()) {
                id = std::to_string(entry["id"].get<int64_t>());
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'id' field, skipping or using empty id: {}", e.what());
        }

        try {
            if (entry.contains("difficulty") && !entry["difficulty"].is_null() && entry["difficulty"].is_string()) {
                difficulty = entry["difficulty"].get<std::string>();
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'difficulty' field, using empty string: {}", e.what());
        }

        try {
            if (entry.contains("difficulty-id") && !entry["difficulty-id"].is_null()) {
                if (entry["difficulty-id"].is_number_integer()) {
                    difficulty_id = entry["difficulty-id"].get<int>();
                } else if (entry["difficulty-id"].is_string()) {
                    try {
                        difficulty_id = std::stoi(entry["difficulty-id"].get<std::string>());
                    } catch (...) {
                        geode::log::info("levels.json: could not parse 'difficulty-id' string, defaulting to 0");
                    }
                }
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'difficulty-id' field, defaulting to 0: {}", e.what());
        }

        try {
            if (entry.contains("stars-amount") && !entry["stars-amount"].is_null()) {
                if (entry["stars-amount"].is_number_integer()) {
                    stars_amount = entry["stars-amount"].get<int>();
                } else if (entry["stars-amount"].is_string()) {
                    try {
                        stars_amount = std::stoi(entry["stars-amount"].get<std::string>());
                    } catch (...) {
                        geode::log::info("levels.json: could not parse 'stars-amount' string, defaulting to 0");
                    }
                }
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'stars-amount' field, defaulting to 0: {}", e.what());
        }

        try {
            if (entry.contains("song-ids") && !entry["song-ids"].is_null() && entry["song-ids"].is_string()) {
                song_ids = entry["song-ids"].get<std::string>();
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'song-ids' field, using empty string: {}", e.what());
        }

        try {
            if (entry.contains("length") && !entry["length"].is_null()) {
                if (entry["length"].is_number_integer()) {
                    length = entry["length"].get<int>();
                } else if (entry["length"].is_string()) {
                    try {
                        length = std::stoi(entry["length"].get<std::string>());
                    } catch (...) {
                        geode::log::info("levels.json: could not parse 'length' string, defaulting to 0");
                    }
                }
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'length' field, defaulting to 0: {}", e.what());
        }

        try {
            if (entry.contains("platformer") && !entry["platformer"].is_null()) {
                if (entry["platformer"].is_boolean()) {
                    isPlatformer = entry["platformer"].get<bool>();
                } else if (entry["platformer"].is_string()) {
                    try {
                        isPlatformer = std::stoi(entry["platformer"].get<std::string>());
                    } catch (...) {
                        geode::log::info("levels.json: could not parse 'platformer' string, defaulting to false");
                    }
                }
            }
        } catch (const std::exception& e) {
            geode::log::info("levels.json: invalid 'platformer' field, defaulting to false", e.what());
        }

        levels.push_back(Level{
            0,
            name,
            id,
            difficulty,
            difficulty_id,
            stars_amount,
            song_ids,
            length,
            isPlatformer
        });
    }

    return levels;
}

void APConnection::saveLevels(const std::vector<Level>& levels, const std::string& path) {
    json j = json::array();

    for (const auto& level : levels) {
        j.push_back({
            {"name", level.name},
            {"id", level.id},
            {"difficulty", level.difficulty},
            {"difficulty-id", level.difficulty_id},
            {"stars-amount", level.stars_amount},
            {"song-ids", level.song_ids},
            {"length", level.length},
            {"platformer", level.isPlatformer}
        });
    }

    std::ofstream out(path);
    out << j.dump(4); // pretty print
}

void APConnection::setLevelProgress(int64_t ap_id, int prog) {
	for (int i = 0; i < randomLevels.size(); ++i) {
        if(std::stoll(randomLevels[i].id) == lvlToID[ap_id]) {
            randomLevels[i].ap_progress = prog;
			geode::log::info("Set Level {} progress to {}", i + 1, prog);
            return;
        }
    }
}

int64_t APConnection::getProgressFromID(int64_t ap_id) {
    for (int i = 0; i < randomLevels.size(); ++i) {
        if(std::stoll(randomLevels[i].id) == lvlToID[ap_id]) {
            return randomLevels[i].ap_progress;
        }
    }
    return 0;
}

void APConnection::buildIDTable(const std::vector<Level>& levels){
    lvlToID.clear();
    IDtoLvl.clear();

    for (size_t i = 0; i < levels.size(); ++i){
        const auto &levelIdStr = levels[i].id;
        if (levelIdStr.empty()) {
            geode::log::info("buildIDTable: skipping level {} because id is empty", i + 1);
            continue;
        }
        try {
            int64_t id = std::stoll(levelIdStr);
            lvlToID.push_back(id);
            IDtoLvl[id] = static_cast<int64_t>(i);
            geode::log::info("Register Level {} -> {}", i + 1, id);
        } catch (const std::exception& e) {
            geode::log::info("buildIDTable: invalid numeric id for level {}: '{}', skipping: {}", i + 1, levelIdStr, e.what());
            continue;
        }
    }

    // lvlToID[0] = ID Level 1
    // lvlToID[1] = ID Level 2

    // IDToLvl[12345] = 1
    // IDToLvl[73263] = 2
}

//TEMP to avoid linker error for global variable
std::vector<Level> APConnection::randomLevels;