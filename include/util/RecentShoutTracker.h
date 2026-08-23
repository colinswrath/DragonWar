#pragma once

#include <unordered_map>

//TODO switch to shout and actor ref formIDs instead of pointers
//TODO serialize
//TODO make sure old entries are cleared. Maybe using actor handles?
struct RecentShout
{
    RecentShout(RE::Actor* dragon, float cooldown, float lastUpdateTime) :
        dragon(dragon), cooldown(cooldown), lastUpdateTime(lastUpdateTime)
    {}

    RE::Actor*    dragon;
    float    cooldown;
    float lastUpdateTime;
};

class RecentShoutTracker
{
public:
    static RecentShoutTracker& GetSingleton();

    void RegisterShout(RE::Actor* actor, RE::TESShout* shout, float lastUpdateTime);

    bool UpdateActorShouts(RE::Actor* actor);

    void RemoveShout(RE::Actor* actor, RE::TESShout* shout);

    RecentShout* GetEntry(RE::Actor* actor, RE::TESShout* shout);

    void RemoveActor(RE::Actor* actor);

    void Clear();

private:
    RecentShoutTracker()  = default;
    ~RecentShoutTracker() = default;

    RecentShoutTracker(const RecentShoutTracker&)            = delete;
    RecentShoutTracker(RecentShoutTracker&&)                 = delete;
    RecentShoutTracker& operator=(const RecentShoutTracker&) = delete;
    RecentShoutTracker& operator=(RecentShoutTracker&&)      = delete;

    std::unordered_map<RE::Actor*, std::unordered_map<RE::TESShout*, RecentShout>> recentShouts;
    mutable std::shared_mutex                                                      mutex_;
};
