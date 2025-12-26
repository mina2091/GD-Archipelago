
#include "APConnection.h"
#include "Archipelago.h"
#include <../../lib/json.hpp>
#include <fstream>
#include <vector>
#include <random>

// for convenience
using json = nlohmann::json;

//data needed from .yaml
//idk if they need to be "= 0" but it works so whatever
auto ap_min_diff = 0;
auto ap_max_diff = 0;
auto ap_level_amount = 0;
auto ap_goal_amount = 0;
auto ap_checks_per_level = 0;
auto ap_starting_level_amount = 0;

auto percentage_for_check = 1;

void APConnection::clearItemCallback() {

}

void APConnection::itemReceivedCallback(int64_t id, bool notify) {

}

void APConnection::locationCheckedCallback(int64_t id) {

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
    percentage_for_check = 100 / ap_checks_per_level;
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
            entry.at("name").get<std::string>(),
            entry.at("id").get<std::string>(),
            entry.at("difficulty").get<std::string>()
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
            {"difficulty", level.difficulty}
        });
    }

    std::ofstream out(path);
    out << j.dump(4); // pretty print
}

//TEMP to avoid linker error for global variable
std::vector<Level> APConnection::randomLevels;