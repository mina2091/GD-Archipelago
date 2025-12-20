
#include "APConnection.h"
#include <../../lib/json.hpp>
#include <fstream>
#include <vector>
#include <random>

// for convenience
using json = nlohmann::json;

void APConnection::clearItemCallback() {

}

void APConnection::itemReceivedCallback(int64_t id, bool notify) {

}

void APConnection::locationCheckedCallback(int64_t id) {

}

std::vector<Level> APConnection::pickRandomLevels(const std::vector<Level>& allLevels, std::size_t count) {
    if (count > allLevels.size()) {
        throw std::runtime_error("Requested more levels than available");
    }

    std::vector<Level> result;
    result.reserve(count);

    std::vector<std::size_t> indices(allLevels.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(indices.begin(), indices.end(), rng);

    for (std::size_t i = 0; i < count; ++i) {
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
