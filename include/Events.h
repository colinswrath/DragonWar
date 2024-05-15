#pragma once

namespace Events
{
    class OnCombatEventHandler : public RE::BSTEventSink<RE::TESCombatEvent>
    {
    protected:
        OnCombatEventHandler()           = default;
        ~OnCombatEventHandler() override = default;

    public:
        OnCombatEventHandler(const OnCombatEventHandler&)            = delete;
        OnCombatEventHandler(OnCombatEventHandler&&)                 = delete;
        OnCombatEventHandler& operator=(const OnCombatEventHandler&) = delete;
        OnCombatEventHandler& operator=(OnCombatEventHandler&&)      = delete;

        static OnCombatEventHandler* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource) override;
        void                     SetRandomCombatStyles(RE::Actor* actor);

        static void Register();
    };
} // namespace Events
