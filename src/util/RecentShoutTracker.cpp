#include "util/RecentShoutTracker.h"

RecentShoutTracker& RecentShoutTracker::GetSingleton()
{
    static RecentShoutTracker singleton;
    return singleton;
}

void RecentShoutTracker::RegisterShout(RE::Actor* actor, RE::TESShout* shout, float lastUpdateTime)
{
    std::unique_lock lk(mutex_);

    auto& shoutMap = recentShouts[actor];

    auto it = shoutMap.find(shout);
    if (it != shoutMap.end()) {
        it->second.lastUpdateTime = lastUpdateTime;
        return;
    }

    auto word = shout->variations[RE::TESShout::VariationIDs::kOne];
    auto cooldown  = word.recoveryTime;

    logger::debug("Adding shout {} to cache. Cooldown {}", shout->GetName(), cooldown);

    shoutMap.emplace(shout, RecentShout(actor, cooldown, lastUpdateTime));
}

RecentShout* RecentShoutTracker::GetEntry(RE::Actor* actor, RE::TESShout* shout)
{
    std::shared_lock lk(mutex_);

    auto actorIt = recentShouts.find(actor);
    if (actorIt == recentShouts.end()) {
        return nullptr;
    }

    auto shoutIt = actorIt->second.find(shout);
    if (shoutIt == actorIt->second.end()) {
        return nullptr;
    }

    return &shoutIt->second;
}

void RecentShoutTracker::RemoveShout(RE::Actor* actor, RE::TESShout* shout)
{
    std::unique_lock lk(mutex_);

    auto actorIt = recentShouts.find(actor);
    if (actorIt == recentShouts.end()) {
        return;
    }

    auto& shoutMap = actorIt->second;
    shoutMap.erase(shout);

    if (shoutMap.empty()) {
        recentShouts.erase(actor);
    }
}

bool RecentShoutTracker::UpdateActorShouts(RE::Actor* actor)
{
    std::shared_lock lk(mutex_);

    auto it = recentShouts.find(actor);
    if (it == recentShouts.end()) {
        logger::debug("No recent shouts");
        return false;
    }

    bool updated            = false;
    const auto currentTime = RE::ProcessLists::GetSingleton()->GetSystemTimeClock();
    auto& shoutMap = it->second;

    std::vector<RE::TESShout*> toErase;

    for (auto& [shout, entry] : shoutMap) {
        float newCooldown = std::clamp(entry.cooldown - (currentTime - entry.lastUpdateTime), 0.0f, entry.cooldown);
        logger::debug("Updated shout {}. Old cooldown {} : new cooldown {}",shout->GetName(), entry.cooldown, newCooldown);

        if (newCooldown == 0.0f) {
            updated = true;
            toErase.push_back(shout);
        }
        else {
            entry.cooldown       = newCooldown;
            entry.lastUpdateTime = currentTime;
        }
    }

    if (!toErase.empty())
    {
        lk.unlock();
        std::unique_lock ulk(mutex_);

        for (auto shout : toErase) {
            shoutMap.erase(shout);
        }
    }

    return updated;
}

void RecentShoutTracker::RemoveActor(RE::Actor* actor)
{
    recentShouts.erase(actor);
}

void RecentShoutTracker::Clear()
{
    recentShouts.clear();
}
