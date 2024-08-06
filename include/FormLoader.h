#pragma once

class FormLoader
{
public:
    static FormLoader* GetSingleton()
    {
        static FormLoader formLoader;
        return &formLoader;
    }

    RE::BGSKeyword* noShoutKeyword;
    RE::TESObjectREFR*  xMarker;
    std::vector<RE::TESCombatStyle*> combatStylePool;

    std::atomic<bool> isRunning{false};

    bool LoadAllForms()
    {
        const auto dragonWarEspName = "DragonWar.esp";
        auto* dataHandler = RE::TESDataHandler::GetSingleton();
        logger::info("Loading forms");
        noShoutKeyword = dataHandler->LookupForm(RE::FormID(0x877A0C), dragonWarEspName)->As<RE::BGSKeyword>();

        //Load combat style pool
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x48C83), "Skyrim.esm")->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF548), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF549), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF54A), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF54B), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF54C), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF54D), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF54E), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF54F), dragonWarEspName)->As<RE::TESCombatStyle>());
        combatStylePool.push_back(dataHandler->LookupForm(RE::FormID(0x8AF550), dragonWarEspName)->As<RE::TESCombatStyle>());

        xMarker = dataHandler->LookupForm(RE::FormID(0x97EFAB), dragonWarEspName)->As<RE::TESObjectREFR>();

        logger::info("Forms loaded");
        return noShoutKeyword ? true : false;
    }
};
