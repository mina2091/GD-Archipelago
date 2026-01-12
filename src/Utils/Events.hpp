#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct LevelInfoClosedEvent : public Event {};

struct LevelInfoClosedFilter : public EventFilter<LevelInfoClosedEvent> {
    using EventFilter::EventFilter;
};
