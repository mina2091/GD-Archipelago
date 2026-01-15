#include "APConnection.hpp"
#include "Archipelago.h"
#include <../../lib/json.hpp>
#include <fstream>
#include <vector>
#include <random>
#include <unordered_map>
#include <numeric>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

#include "../Layer/APLogInLayer.hpp"

// for convenience
using json = nlohmann::json;

//data needed from .yaml
auto ap_min_diff = 1;
auto ap_max_diff = 1;
auto ap_level_amount = 100; //test value for now
auto ap_goal_amount = 1; //test value for now
auto ap_checks_per_level = 0;
auto ap_starting_level_amount = 0;
auto ap_percentage_for_check = 20; //test value for now

int percentagesGetFromInitArray[100];  //we have to change that
int playerProgressPerLevelArray[100];
int currentFinishedLevels = 0;

namespace APConnection {
    std::vector<int64_t> lvlToID;
    std::unordered_map<int64_t, int64_t> IDtoLvl;
}

// local flags for slot data arrival and init completion
namespace {
    std::atomic<bool> g_min_received{false};
    std::atomic<bool> g_max_received{false};
    std::atomic<bool> g_init_done{false};
    std::atomic<bool> g_init_timedout{false};
}

void APConnection::clearItemCallback() {
    geode::log::info("APConnection::clearItemCallback");
}

int APConnection::getLevelAmount(){
    return ap_level_amount;
}

void APConnection::itemReceivedCallback(int64_t id, bool notify){
    geode::log::info("Received Item ID: {}", id);

    /*
    if (id == 356){         //this honestly might be the worst code ever written
        clearTable();
        //int* percentagesGetFromInitArray = new int[ap_level_amount];  //TODO: create array with length of level_amount
    }
    */

    //[GD-Archipelago]: APConnection::itemReceivedCallback, 356 - Level 1
    //[GD-Archipelago]: APConnection::itemReceivedCallback, 456 - Level 2

    id = (id - 56)/100-3;
	// (356 - 56) / 100 - 3 = 0
	// (456 - 56) / 100 - 3 = 1

    for (int i = 0; i < 100; i++) {
        if (i == id) {
            percentagesGetFromInitArray[i] += 1;
            geode::log::info("Level {}: +{}%. (Now: {}%)", id + 1, ap_percentage_for_check, ap_percentage_for_check * percentagesGetFromInitArray[i]);
            return;
        }
    }

    setLevelProgress(id, randomLevels[id].ap_progress);
}
/*
	auto new_progress = getProgressFromID(id) + ap_percentage_for_check;
	geode::log::info("Updated Level {} progress from {}% to {}%", id + 1, getProgressFromID(id), new_progress);
	setLevelProgress(id, new_progress);
}
*/

void APConnection::locationCheckedCallback(int64_t id) {
    geode::log::info("Received Location Check ID: {}", id);

    /*
    20:08:16 INFO  [GD-Archipelago]: Received Location Check ID: 257
    20:08:16 INFO  [GD-Archipelago]: Received Location Check ID: 259
    20:08:16 INFO  [GD-Archipelago]: Received Location Check ID: 357
    */

    auto lvl_player_progress = id/100-2;

    for (int i = 0; i < 100; i++) {
        if (i == lvl_player_progress) {
            playerProgressPerLevelArray[i] += 1;
            geode::log::info("Added Player Progression to Level {}", lvl_player_progress+1);
            return;
        }
    }

    //275
    if (id%100 == 75){
        addToCurrentFinishedLevels();
        geode::log::info("added to finished_levels because {} was X75", id);
    }
}

//convert into local variables
void APConnection::setMinDiff(int i){
    ap_min_diff = i;
    g_min_received.store(true);
    geode::log::info("Min Diff: {}", i);
}

void APConnection::setMaxDiff(int i){
    ap_max_diff = i;
    g_max_received.store(true);
    geode::log::info("Max Diff: {}", i);
}

void APConnection::setLevelAmount(int i){
    ap_level_amount = i;
	geode::log::info("Level Amount: {}", i);
}

void APConnection::setGoalAmount(int i){
    ap_goal_amount = i;
	geode::log::info("Goal Amount: {}", i);
}

void APConnection::setChecksPerLevel(int i){
    ap_checks_per_level = i;
    ap_percentage_for_check = 100 / i;
	geode::log::info("Checks Per Level: {}, Percentage per Check: {}%", i, ap_percentage_for_check);
}

int APConnection::getPPC(){
   return ap_percentage_for_check;
}

void APConnection::setStartingLevelAmount(int i){
    ap_starting_level_amount = i;
	geode::log::info("Starting Level Amount: {}", i);
}

void APConnection::clearTable(){
    //clear previous loaded content for percentages
    std::fill(
        std::begin(percentagesGetFromInitArray),
        std::end(percentagesGetFromInitArray),
        0
    );

    std::fill(
        std::begin(playerProgressPerLevelArray),
        std::end(playerProgressPerLevelArray),
        0
    );
    geode::log::info("Tables cleared");
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

    std::size_t i = 0;
    std::size_t validAmount = 0;
    while (validAmount < static_cast<std::size_t>(ap_level_amount) && i < indices.size()) {
        const auto &cand = allLevels[indices[i]];
        if (!cand.isPlatformer
            && cand.difficulty_id >= ap_min_diff
            && cand.difficulty_id <= ap_max_diff
            ) {
            result.push_back(cand);
            validAmount++;
        }
        ++i;
    }

    // If not enough, fill with any remaining without duplicates
    if (result.size() < static_cast<std::size_t>(ap_level_amount)) {
        geode::log::info("pickRandomLevels: only {} matching levels found, filling with remaining", result.size());
        for (std::size_t idx = 0; idx < indices.size() && result.size() < static_cast<std::size_t>(ap_level_amount); ++idx) {
            const auto &cand = allLevels[indices[idx]];
            bool already = false;
            for (const auto &r : result) {
                if (r.id == cand.id) { already = true; break; }
            }
            if (!already) result.push_back(cand);
        }
    }

    APConnection::buildIDTable(result);

    return result;
}

bool APConnection::getIsFinished(int ap_id){
    return APConnection::randomLevels[ap_id].isFinished;
}

void APConnection::setIsFinished(int ap_id){
    APConnection::randomLevels[ap_id].isFinished = true;
}

void APConnection::addToCurrentFinishedLevels(){
    currentFinishedLevels += 1;
    geode::log::info("added 1 to finishedlevels, now: {}", currentFinishedLevels);
	geode::log::info("levels needed to finish: {}", ap_goal_amount);    //TODO: goal amount still sometimes takes hard-coded value
}

void APConnection::checkForGoalAmount(){
    if (currentFinishedLevels == ap_goal_amount){
        geode::log::info("finished!");
        AP_StoryComplete();
    }
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

    int counter = 0;
    int counterPlayer = 0;

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
        bool isFinished = false;

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
            percentagesGetFromInitArray[counter++] * ap_percentage_for_check,
            name,
            id,
            difficulty,
            difficulty_id,
            stars_amount,
            song_ids,
            length,
            isPlatformer,
            isFinished,
            playerProgressPerLevelArray[counterPlayer++] * ap_percentage_for_check
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
    randomLevels[ap_id].ap_progress = prog;
    geode::log::info("Set Level {} progress to {}%", ap_id + 1, prog);
    return;
}

int64_t APConnection::getProgressFromID(int64_t ap_id) {
    return randomLevels[ap_id].ap_progress;
}

void APConnection::setCurrentProgress(int64_t ap_id, int prog){
    randomLevels[ap_id].playerProgress = prog;
    geode::log::info("Set Level {} player progress to {}%", ap_id + 1, prog);
}

int64_t APConnection::getCurrentProgressFromID(int64_t ap_id){
    return randomLevels[ap_id].playerProgress;
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

void APConnection::initOnConnect() {
    // run registration on a background thread and wait for min/max slot data before registering
    g_init_done.store(false);
    g_init_timedout.store(false);
    std::thread([]() {
        geode::log::info("APConnection::initOnConnect: waiting for slot data (min/max) before registering levels...");
        using namespace std::chrono_literals;
        auto start = std::chrono::steady_clock::now();
        const auto timeout = 5s;
        bool noTimeout = true;

        while (!(g_min_received.load() && g_max_received.load())) {
            if (std::chrono::steady_clock::now() - start > timeout) {
                geode::log::info("APConnection::initOnConnect: timeout waiting for slot data, signalling timeout to main thread");
                noTimeout = false;
                g_init_timedout.store(true);
                break;
            }
            std::this_thread::sleep_for(100ms);
        }
        if (noTimeout) {
            try {
                AP_RoomInfo roomInfo{};
                AP_GetRoomInfo(&roomInfo);
                auto seed = roomInfo.seed_name;
                auto oldLevels = fmt::format("{}.json",seed);
                auto outPath = geode::Mod::get()->getSaveDir() / oldLevels;

                log::info("Seed: {}", seed);
                //if there's no save data for this multiworld create new randomized levels
                if (!std::filesystem::exists(outPath)) {

                    geode::log::info("No multiworld data found. Randomizing levels.");

                    auto levelsPath = geode::Mod::get()->getResourcesDir() / "levels.json";
                    auto allLevels = loadLevels(levelsPath.string());
                    randomLevels = pickRandomLevels(allLevels);

                    auto savedLevels = fmt::format("{}.json",seed);
                    auto savePath = geode::Mod::get()->getSaveDir() / savedLevels;

                    saveLevels(randomLevels, savePath.string());
                    geode::log::info("Saved random levels to: {}", savePath.string());
                }
                //else load randomized levels from existing directory
                else {
                    geode::log::info("Found Multiworld data. Retrieving randomized levels.");
                    randomLevels = loadLevels(outPath.string());

                    //important for level count in ListView
                    APConnection::buildIDTable(randomLevels);
                }

            } catch (const std::exception& e) {
                geode::log::info("APConnection::initOnConnect: failed to initialize levels: {}", e.what());
            }
            g_init_done.store(true);
        }
    }).detach();

}

bool APConnection::isInitComplete() {
    return g_init_done.load();
}

bool APConnection::isInitTimedOut() {
    return g_init_timedout.load();
}

void APConnection::resetData() {
    geode::log::info("APConnection::resetAfterTimeout: cleaning up after timeout...");

    // try to stop the client if possible
    try {
        AP_Shutdown();
    } catch (...) {
        geode::log::info("APConnection::resetAfterTimeout: AP_Shutdown threw or not available");
    }

    // unregister callbacks (set to nullptr)
    try { AP_SetItemClearCallback(nullptr); } catch (...) {}
    try { AP_SetItemRecvCallback(nullptr); } catch (...) {}
    try { AP_SetLocationCheckedCallback(nullptr); } catch (...) {}

    // reset internal flags
    g_min_received.store(false);
    g_max_received.store(false);
    g_init_done.store(false);
    g_init_timedout.store(false);

    // clear registration tables so re-init starts fresh
    lvlToID.clear();
    IDtoLvl.clear();
    randomLevels.clear();
    clearTable();
    currentFinishedLevels = 0;

    geode::log::info("APConnection::resetAfterTimeout: cleanup complete");
}

//TEMP to avoid linker error for global variable
std::vector<Level> APConnection::randomLevels;