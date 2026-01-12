#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class APCheckLayer : public CCLayer {
public:
    static void show(std::string const& msg);

private:
    static APCheckLayer* create(std::string const& msg);
    bool init(std::string const& msg);
};
