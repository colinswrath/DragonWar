#pragma once
#include "FormLoader.h"
#include "util/RecentShoutTracker.h"

// Credit note: po3 for Offensive spell AI in po3 tweaks.
// Code isnt the same, but this hooks the same vtable function, and I used it as a reference to do so.
namespace Hooks
{
    struct ExecuteHandler
    {
        static bool thunk(RE::VoiceSpellFireHandler* a_this, RE::Actor& a_actor, const RE::BSFixedString& a_tag)
        {
            const auto result = func(a_this, a_actor, a_tag);

            if (result && !a_actor.IsPlayerRef()) {
                if (const auto shout = a_actor.GetCurrentShout()) {
                    logger::info("{} just used {}", a_actor.GetName(), shout->GetName());

                    auto variationSpell01 = shout ? shout->variations[0].spell : nullptr;
                    if (variationSpell01 && variationSpell01->HasKeyword(FormLoader::GetSingleton()->noShoutKeyword)) {

                        RE::Actor* actor   = &a_actor;
                        auto       handle  = actor->CreateRefHandle();

                        const auto currentTime = RE::ProcessLists::GetSingleton()->GetSystemTimeClock();
                        auto&      tracker = RecentShoutTracker::GetSingleton();
                        tracker.RegisterShout(actor, shout, currentTime);

                        auto controller = a_actor.GetActorRuntimeData().combatController;
                        auto equipped = controller->inventory->equippedItems;
                        auto& invArray   = controller->inventory->inventoryItems[0];

                        for (auto it = invArray.begin(); it != invArray.end();) {
                            auto& invItem = *it;
                            if (invItem && invItem->GetType() == RE::CombatInventoryItem::TYPE::kShout) {
                                auto invShout = invItem->item->As<RE::TESShout>();

                                if (invShout->GetFormID() == shout->GetFormID()) {
                                    logger::debug("Unequipped {}", shout->GetName());
                                    invItem->Unequip(controller);
                                    invArray.erase(it);
                                    break;
                                }
                            }
                            ++it;
                        }
                    }
                }
            }

            return result;
        }

        inline static REL::Relocation<decltype(thunk)> func;
        inline static std::size_t                      idx{ 0x1 };
    };

    template <class T>
    class CheckShouldEquip
    {
    public:
        static void Install(REL::VariantID a_vtable_id)
        {
            logger::info("Installing Check Equip: {} hook", typeid(T).name());

            REL::Relocation<std::uintptr_t> vtbl{ a_vtable_id };
            func = vtbl.write_vfunc(0xF, thunk);

            logger::info("Installed Check Equip: {} hook", typeid(T).name());
        }

    private:
        static bool thunk(RE::CombatInventoryItemMagicT<RE::CombatInventoryItemShout, T>* a_this, RE::CombatController* a_controller)
        {
            auto result = func(a_this, a_controller);

            if (result && a_this->IsValid() && a_controller) {
                auto attacker = a_controller->handleCount && a_controller->handleCount > 0 ? a_controller->cachedAttacker : nullptr;

                if (!attacker)
                {
                    attacker = a_controller->attackerHandle.get();
                }

                if (attacker && !attacker->IsPlayer() && attacker->HasKeywordString("ActorTypeDragon"sv)) {
                    logger::debug("-----------------------");
                    logger::debug("Check equip {}", attacker->GetName());
                    auto shout = a_this->item->As<RE::TESShout>();

                    auto variationSpell01 = shout ? shout->variations[0].spell : nullptr;
                    if (variationSpell01 && variationSpell01->HasKeyword(FormLoader::GetSingleton()->noShoutKeyword)) {
                        logger::debug("Check timeout {}",variationSpell01->GetName());

                        auto& tracker = RecentShoutTracker::GetSingleton();

                        auto entry = tracker.GetEntry(attacker.get(), shout);
                        if (entry) {
                            logger::debug("Shout {} entry under cooldown. Do not equip",shout->GetName());
                            return false;
                        }
                    }

                }
            }

            return result;
        }

        inline static REL::Relocation<decltype(thunk)> func;
    };

    class OnActorUpdate
    {

    public:
        static void Install()
        {
            logger::info("Installing Actor update");
            REL::Relocation<std::uintptr_t> actorUpdate{ REL::RelocationID(36357, 37348), REL::Relocate(0x6D3, 0x674) };

            auto& trampoline = SKSE::GetTrampoline();
            _Update          = trampoline.write_call<5>(actorUpdate.address(), Update);
            logger::info("Hook installed");
        }

        static std::string intToHexString(int value)
        {
            std::stringstream stream;
            stream << std::hex << value; // Convert integer to hexadecimal string
            return stream.str();         // Return the string representation
        }

    private:
        static void Update(RE::Actor* a_actor, float a_zPos, RE::TESObjectCELL* a_cell)
        {
            if (a_actor && !a_actor->IsPlayer() && a_actor->HasKeywordString("ActorTypeDragon"sv)) {
                logger::debug("-----------------------");
                logger::debug("Actor update {}", a_actor->GetName());
                logger::debug("Actor update formId {}", a_actor->GetFormID());

                //TODO IDEA
                //Search combat inventory, if shout is present and meets conditions then cast spell immediate. Does that trigger the shout event?
                //Target should just be combat target

                //auto* formLoader = FormLoader::GetSingleton();
                auto* combatGroup = a_actor->GetCombatGroup();
                auto target      = a_actor->GetActorRuntimeData().currentCombatTarget;
                auto  controller  = a_actor->GetActorRuntimeData().combatController;
                //TEST ALWAYS TARGET PLAYER
                /*if (combatGroup && target) {

                    logger::debug("Current target before {}", target.get().get()->GetName());

                    for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
                        if (it->targetHandle && it->targetHandle.get().get()) {
                            logger::debug("Check target: {}", it->targetHandle.get().get()->GetName());
                            if (it->targetHandle.get().get()->IsPlayerRef()) {
                                logger::debug("Updating target to player");
                                a_actor->GetActorRuntimeData().currentCombatTarget = it->targetHandle.get().get();
                                controller->targetHandle                           = it->targetHandle;
                                controller->cachedTarget                           = it->targetHandle.get();
                            }
                            break;
                        }
                        continue;
                    }
                    target = a_actor->GetActorRuntimeData().currentCombatTarget;
                    if (target) {
                        logger::debug("Current target after {}", a_actor->GetActorRuntimeData().currentCombatTarget.get().get()->GetName());
                    }
                }*/

                //Iterate over stored cached
                auto& tracker = RecentShoutTracker::GetSingleton();
                bool cooldownExpired = tracker.UpdateActorShouts(a_actor);

                controller = a_actor->GetActorRuntimeData().combatController;
                
                if (controller) {

                    auto equipped = controller->inventory->equippedItems;

                    for (const auto item : equipped) {
                        if (item.item && item.item->GetType() == RE::CombatInventoryItem::TYPE::kShout) {
                            logger::debug("Equipped shout {} - Score: {}", item.item.get()->item->GetName(), item.item.get()->itemScore);
                        }
                    }

                    for (auto& invArray : controller->inventory->inventoryItems) {
                        for (auto it = invArray.begin(); it != invArray.end();) {
                            auto& invItem = *it;
                            if (invItem && invItem->GetType() == RE::CombatInventoryItem::TYPE::kShout) {
                                auto invShout = invItem->item->As<RE::TESShout>();
                                logger::debug("Inventory: {} - Score: {}", invShout->GetName(), invItem->itemScore);

                                auto variationSpell01 = invShout ? invShout->variations[0].spell : nullptr;
                                if (variationSpell01 && variationSpell01->HasKeyword(FormLoader::GetSingleton()->noShoutKeyword)) {
                                    auto shoutCaster = a_actor->GetActorRuntimeData().magicCasters[RE::Actor::SlotTypes::kPowerOrShout];
                                    
                                    //shoutCaster->CastSpellImmediate(invShout->variations[RE::TESShout::VariationIDs::kThree].spell, false, controller->cachedTarget.get(), 1.0f, false, 0.0f, a_actor);
                                    invItem->Equip(controller);
                                }
                            }
                            ++it;
                        }
                    }
                }

                if (cooldownExpired) {
                    controller = a_actor->GetActorRuntimeData().combatController;
                    if (controller) {
                        logger::info("Detected cooldown expire. Trigger inventory rebuild");
                        controller->inventory->dirty = true;
                    }
                }
            }

            return _Update(a_actor, a_zPos, a_cell);
        }

        inline static REL::Relocation<decltype(Update)> _Update;
    };

    void Install()
    {
        CheckShouldEquip<RE::CombatMagicCasterTargetEffect>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterTargetEffect_[0]);
        CheckShouldEquip<RE::CombatInventoryItemShout>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterOffensive_[0]);
        CheckShouldEquip<RE::CombatMagicCasterSummon>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterSummon_[0]);
        CheckShouldEquip<RE::CombatMagicCasterReanimate>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterReanimate_[0]);
        CheckShouldEquip<RE::CombatMagicCasterRestore>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterRestore_[0]);
        CheckShouldEquip<RE::CombatMagicCasterScript>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterScript_[0]);
        CheckShouldEquip<RE::CombatMagicCasterStagger>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterStagger_[0]);
        CheckShouldEquip<RE::CombatMagicCasterWard>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterWard_[0]);
        CheckShouldEquip<RE::CombatMagicCasterBoundItem>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterBoundItem_[0]);
        CheckShouldEquip<RE::CombatMagicCasterCloak>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterCloak_[0]);
        CheckShouldEquip<RE::CombatMagicCasterDisarm>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterDisarm_[0]);
        CheckShouldEquip<RE::CombatMagicCasterInvisibility>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterInvisibility_[0]);
        CheckShouldEquip<RE::CombatMagicCasterLight>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterLight_[0]);
        CheckShouldEquip<RE::CombatMagicCasterParalyze>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterParalyze_[0]);
        CheckShouldEquip<RE::CombatMagicCasterWard>::Install(RE::VTABLE_CombatInventoryItemMagicT_CombatInventoryItemShout_CombatMagicCasterWard_[0]);


        logger::info("\t\tInstalled magic caster vtable hooks"sv);
        stl::write_vfunc<RE::VoiceSpellFireHandler, ExecuteHandler>();
        OnActorUpdate::Install();
        logger::info("Installed Actor Update hook");
    }
}
