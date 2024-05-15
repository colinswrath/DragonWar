#include "Events.h"
#include "Settings.h"
#include "FormLoader.h"

namespace Events
{
    OnCombatEventHandler* OnCombatEventHandler::GetSingleton()
    {
        static OnCombatEventHandler singleton;
        return &singleton;
    }

    //Credit: ThirdEyeSqueegee. Event code based on previous code written by ThirdEyeSqueegee
    RE::BSEventNotifyControl OnCombatEventHandler::ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource)
    {
        if (!a_event || !a_event->actor || !a_event->targetActor)
            return RE::BSEventNotifyControl::kContinue;

        if (a_event->newState != RE::ACTOR_COMBAT_STATE::kCombat)
            return RE::BSEventNotifyControl::kContinue;

        if (const auto actor = a_event->actor->As<RE::Actor>()) {
            if (actor->HasKeywordString("ActorTypeDragon"sv)) {
                if (!actor->HasKeywordString("DW_ActorTypeDragonCSExclude"sv)) {
                    if (a_event->targetActor && a_event->targetActor->IsPlayerRef()) {
                        SetRandomCombatStyles(actor);       
                    }
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void OnCombatEventHandler::SetRandomCombatStyles(RE::Actor* actor) {

        auto settings = Settings::GetSingleton();
        auto stylePool = FormLoader::GetSingleton()->combatStylePool;
        const auto override = settings->OverrideSliders;
        logger::debug("\tini override: {}", override);

        logger::debug("\tStyle Index {}", settings->currentStyleIndex);
        auto* chosenStyle = stylePool[settings->currentStyleIndex];

        if (settings->currentStyleIndex == stylePool.size() - 1) {
            settings->currentStyleIndex = 0;
        }
        else {
            settings->currentStyleIndex = settings->currentStyleIndex + 1;
        }

        //Randomize the style and then add it to the dragon

        std::random_device rd;

        const auto dive_bomb_chance     = override ? settings->fDiveBombChance : clib_util::RNG(rd()).Generate<float>(settings->fDiveBombChanceMin, settings->fDiveBombChanceMax);
        const auto flying_attack_chance = override ? settings->fFlyingAttackChance : clib_util::RNG(rd()).Generate<float>(settings->fFlyingAttackChanceMin, settings->fFlyingAttackChanceMax);
        const auto ground_attack_chance = override ? settings->fGroundAttackChance : clib_util::RNG(rd()).Generate<float>(settings->fGroundAttackChanceMin, settings->fGroundAttackChanceMax);
        const auto ground_attack_mult   = override ? settings->fGroundAttackMult : clib_util::RNG(rd()).Generate<float>(settings->fGroundAttackMultMin, settings->fGroundAttackMultMax);
        const auto hover_chance         = override ? settings->fHoverChance : clib_util::RNG(rd()).Generate<float>(settings->fHoverChanceMin, settings->fHoverChanceMax);
        const auto hover_mult           = override ? settings->fHoverMult : clib_util::RNG(rd()).Generate<float>(settings->fHoverMultMin, settings->fHoverMultMax);
        const auto perch_attack_chance  = override ? settings->fPerchAttackChance : clib_util::RNG(rd()).Generate<float>(settings->fPerchAttackChanceMin, settings->fPerchAttackChanceMax);
        const auto perch_attack_mult    = override ? settings->fPerchAttackMult : clib_util::RNG(rd()).Generate<float>(settings->fPerchAttackMultMin, settings->fPerchAttackMultMax);
        const auto equip_unarmed_mult   = override ? settings->fEquipUnarmedMult : clib_util::RNG(rd()).Generate<float>(settings->fEquipUnarmedMultMin, settings->fEquipUnarmedMultMax);
        const auto equip_shout_mult     = override ? settings->fEquipShoutMult : clib_util::RNG(rd()).Generate<float>(settings->fEquipShoutMultMin, settings->fEquipShoutMultMax);

        chosenStyle->flightData.diveBombChance = dive_bomb_chance;
        logger::debug("\tSet dive bomb chance to {}", dive_bomb_chance);
        chosenStyle->flightData.flyingAttackChance = flying_attack_chance;
        logger::debug("\tSet flying attack chance to {}", flying_attack_chance);
        chosenStyle->flightData.groundAttackChance = ground_attack_chance;
        logger::debug("\tSet ground attack chance to {}", ground_attack_chance);
        chosenStyle->flightData.groundAttackTimeMult = ground_attack_mult;
        logger::debug("\tSet ground attack time mult to {}", ground_attack_mult);
        chosenStyle->flightData.hoverChance = hover_chance;
        logger::debug("\tSet hover chance to {}", hover_chance);
        chosenStyle->flightData.hoverTimeMult = hover_mult;
        logger::debug("\tSet hover time mult to {}", hover_mult);
        chosenStyle->flightData.perchAttackChance = perch_attack_chance;
        logger::debug("\tSet perch attack chance to {}", perch_attack_chance);
        chosenStyle->flightData.perchAttackTimeMult = perch_attack_mult;
        logger::debug("\tSet perch attack time mult to {}", perch_attack_mult);
        chosenStyle->generalData.unarmedScoreMult = equip_unarmed_mult;
        logger::debug("\tSet unarmed score mult to {}", equip_unarmed_mult);
        chosenStyle->generalData.shoutScoreMult = equip_shout_mult;
        logger::debug("\tSet shout score mult to {}", equip_shout_mult);

        logger::debug("\tCombat style before: {}", std::to_string(actor->GetActorRuntimeData().combatController->combatStyle->GetFormID()));

        actor->GetActorRuntimeData().combatController->combatStyle = chosenStyle;
        logger::debug("\tCombat style after: {}", std::to_string(actor->GetActorRuntimeData().combatController->combatStyle->GetFormID()));

        logger::debug("\tRandomized {}'s flight data and equip sliders", actor->GetName());
    }

    void OnCombatEventHandler::Register()
    {
        const auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->AddEventSink(GetSingleton());
        logger::info("Registered OnCombat event handler");
    }
}
