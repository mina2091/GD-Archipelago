#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct LevelInfoClosedEvent : public GlobalEvent<LevelInfoClosedEvent, bool(int value), std::string>
{
    using GlobalEvent = GlobalEvent;
};
