
#include "APConnection.hpp"
#include "Archipelago.h"
#include <../../lib/json.hpp>
#include <fstream>
#include <vector>
#include <random>
#include <unordered_map>

// for convenience
using json = nlohmann::json;

//data needed from .yaml
//idk if they need to be "= 0" but it works so whatever
auto ap_min_diff = 0;
auto ap_max_diff = 0;
auto ap_level_amount = 20; //test value for now
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

    for (std::size_t i = 0; i < ap_level_amount; ++i) {
        result.push_back(allLevels[indices[i]]);
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

    std::vector<Level> levels;
    levels.reserve(j.size());

    for (const auto& entry : j) {
        levels.push_back(Level{
            0,
            entry.at("name").get<std::string>(),
            entry.at("id").get<std::string>(),
            entry.at("difficulty").get<std::string>(),
            entry.at("difficulty-id").get<int>(),
            entry.at("stars-amount").get<int>()
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
            {"stars-amount", level.stars_amount}
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
        int64_t id = std::stoll(levels[i].id);
        lvlToID.push_back(id);

        IDtoLvl[id] = static_cast<int64_t>(i);
        geode::log::info("Register Level {} -> {}", i + 1, id);
    }

    // lvlToID[0] = ID Level 1
    // lvlToID[1] = ID Level 2

    // IDToLvl[12345] = 1
    // IDToLvl[73263] = 2
}

//TEMP to avoid linker error for global variable
std::vector<Level> APConnection::randomLevels;