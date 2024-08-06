#pragma once
#include "FormLoader.h"

// Credit note: po3 for Offensive spell AI in po3 tweaks.
// Code isnt the same, but this hooks the same vtable function, and I used it as a reference to do so.
namespace Hooks
{
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


            if (result && a_this->IsValid()) {
                const auto attacker = a_controller->handleCount ? a_controller->cachedAttacker : a_controller->attackerHandle.get();
                if (attacker && !attacker->IsPlayer() && attacker->HasKeywordString("ActorTypeDragon"sv)) {
                    logger::debug("-----------------------");
                    logger::debug("Check equip {}", attacker->GetName());
                    auto shout = a_this->item->As<RE::TESShout>();

                    auto variationSpell01 = shout ? shout->variations[0].spell : nullptr;

                    if (variationSpell01 && variationSpell01->HasKeyword(FormLoader::GetSingleton()->noShoutKeyword)) {
                        return false;
                    }

                }
            }

            return result;
        }

        inline static REL::Relocation<decltype(thunk)> func;
    };

    class OnActorUpdate
    {

            //REL::Relocation<std::uintptr_t> EvaluateProcessHook{ REL::RelocationID(36407, 37401), REL::Relocate(0x44, 0x44) };

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

                auto* formLoader = FormLoader::GetSingleton();

                bool expected = false;
                if (formLoader->isRunning.compare_exchange_strong(expected, true)) {
                    auto combatTarget = a_actor->GetActorRuntimeData().currentCombatTarget;

                    if (combatTarget) {
                        logger::debug("Combat target: {}", combatTarget.get()->GetName());

                        if (const auto process = a_actor->GetActorRuntimeData().currentProcess) {
                            auto currentPackage = process->currentPackage.data;

                            auto package = process->currentPackage.package;
                            if (package) {
                                logger::debug("Current package {}", intToHexString(package->GetFormID()));
                            }

                            if (process->currentPackage.target && process->currentPackage.target.get()->GetFormID() == a_actor->GetFormID()) {
                                logger::debug("Package target was self, not swapping");
                            }
                            else {
                                if (process->currentPackage.target) {
                                    logger::debug("Package target was {}", process->currentPackage.target.get()->GetName());
                                }

                                auto xMarker = formLoader->xMarker;

                                if (xMarker) {
                                    auto targetActorPtr = combatTarget.get();
                                    if (targetActorPtr != nullptr) {
                                        auto targetActor = targetActorPtr.get();
                                        if (targetActor) {
                                            xMarker->MoveTo(targetActor);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else {
                        logger::debug("No Combat target");
                    }
                    formLoader->isRunning = false;
                }
                else {
                    logger::debug("Xmarker movement already in progress. Skipping");
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

        OnActorUpdate::Install();
        logger::info("Installed Actor Update hook");
    }
}
