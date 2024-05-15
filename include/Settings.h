class Settings
{
public:

    inline static bool  OverrideSliders;
    inline static bool  DebugLogs;
    inline static float fDiveBombChance;
    inline static float fDiveBombChanceMin;
    inline static float fDiveBombChanceMax;
    inline static float fFlyingAttackChance;
    inline static float fFlyingAttackChanceMin;
    inline static float fFlyingAttackChanceMax;
    inline static float fGroundAttackChance;
    inline static float fGroundAttackChanceMin;
    inline static float fGroundAttackChanceMax;
    inline static float fGroundAttackMult;
    inline static float fGroundAttackMultMin;
    inline static float fGroundAttackMultMax;
    inline static float fHoverChance;
    inline static float fHoverChanceMin;
    inline static float fHoverChanceMax;
    inline static float fHoverMult;
    inline static float fHoverMultMin;
    inline static float fHoverMultMax;
    inline static float fPerchAttackChance;
    inline static float fPerchAttackChanceMin;
    inline static float fPerchAttackChanceMax;
    inline static float fPerchAttackMult;
    inline static float fPerchAttackMultMin;
    inline static float fPerchAttackMultMax;
    inline static float fEquipUnarmedMult;
    inline static float fEquipUnarmedMultMin;
    inline static float fEquipUnarmedMultMax;
    inline static float fEquipShoutMult;
    inline static float fEquipShoutMultMin;
    inline static float fEquipShoutMultMax;

    inline static std::int32_t currentStyleIndex;

    static Settings* GetSingleton()
    {
        static Settings settings;
        return &settings;
    }

    static void LoadSettings()
    {
        logger::info("Loading settings");

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(R"(.\Data\SKSE\Plugins\DragonWar.ini)");

        OverrideSliders     = ini.GetBoolValue("", "bOverrideCombatStyleSliders", false);
        DebugLogs           = ini.GetBoolValue("", "bDebugLogs", false);
        logger::info("\tDebug log: {}", DebugLogs);
        if (DebugLogs) {
            spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
            logger::debug("Debug logging enabled");
        }

        fDiveBombChance        = static_cast<float>(ini.GetDoubleValue("", "fDiveBombChance", 0.0f));
        fDiveBombChanceMin     = static_cast<float>(ini.GetDoubleValue("", "fDiveBombChanceMin", 0.0f));
        fDiveBombChanceMax     = static_cast<float>(ini.GetDoubleValue("", "fDiveBombChanceMax", 1.0f));
        fFlyingAttackChance    = static_cast<float>(ini.GetDoubleValue("", "fFlyingAttackChance", 0.0f));
        fFlyingAttackChanceMin = static_cast<float>(ini.GetDoubleValue("", "fFlyingAttackChanceMin", 0.0f));
        fFlyingAttackChanceMax = static_cast<float>(ini.GetDoubleValue("", "fFlyingAttackChanceMax", 1.0f));
        fGroundAttackChance    = static_cast<float>(ini.GetDoubleValue("", "fGroundAttackChance", 0.0f));
        fGroundAttackChanceMin = static_cast<float>(ini.GetDoubleValue("", "fGroundAttackChanceMin", 0.0f));
        fGroundAttackChanceMax = static_cast<float>(ini.GetDoubleValue("", "fGroundAttackChanceMax", 1.0f));
        fGroundAttackMult      = static_cast<float>(ini.GetDoubleValue("", "fGroundAttackMult", 0.0f));
        fGroundAttackMultMin   = static_cast<float>(ini.GetDoubleValue("", "fGroundAttackMultMin", 0.0f));
        fGroundAttackMultMax   = static_cast<float>(ini.GetDoubleValue("", "fGroundAttackMultMax", 1.0f));
        fHoverChance           = static_cast<float>(ini.GetDoubleValue("", "fHoverChance", 0.0f));
        fHoverChanceMin        = static_cast<float>(ini.GetDoubleValue("", "fHoverChanceMin", 0.0f));
        fHoverChanceMax        = static_cast<float>(ini.GetDoubleValue("", "fHoverChanceMax", 1.0f));
        fHoverMult             = static_cast<float>(ini.GetDoubleValue("", "fHoverMult", 0.0f));
        fHoverMultMin          = static_cast<float>(ini.GetDoubleValue("", "fHoverMultMin", 0.0f));
        fHoverMultMax          = static_cast<float>(ini.GetDoubleValue("", "fHoverMultMax", 1.0f));
        fPerchAttackChance     = static_cast<float>(ini.GetDoubleValue("", "fPerchAttackChance", 0.0f));
        fPerchAttackChanceMin  = static_cast<float>(ini.GetDoubleValue("", "fPerchAttackChanceMin", 0.0f));
        fPerchAttackChanceMax  = static_cast<float>(ini.GetDoubleValue("", "fPerchAttackChanceMax", 1.0f));
        fPerchAttackMult       = static_cast<float>(ini.GetDoubleValue("", "fPerchAttackMult", 0.0f));
        fPerchAttackMultMin    = static_cast<float>(ini.GetDoubleValue("", "fPerchAttackMultMin", 0.0f));
        fPerchAttackMultMax    = static_cast<float>(ini.GetDoubleValue("", "fPerchAttackMultMax", 1.0f));
        fEquipUnarmedMult      = static_cast<float>(ini.GetDoubleValue("", "fEquipUnarmedMult", 0.0f));
        fEquipUnarmedMultMin   = static_cast<float>(ini.GetDoubleValue("", "fEquipUnarmedMultMin", 0.0f));
        fEquipUnarmedMultMax   = static_cast<float>(ini.GetDoubleValue("", "fEquipUnarmedMultMax", 10.0f));
        fEquipShoutMult        = static_cast<float>(ini.GetDoubleValue("", "fEquipShoutMult", 0.0f));
        fEquipShoutMultMin     = static_cast<float>(ini.GetDoubleValue("", "fEquipShoutMultMin", 0.0f));
        fEquipShoutMultMax     = static_cast<float>(ini.GetDoubleValue("", "fEquipShoutMultMax", 10.0f));
        currentStyleIndex      = 0;
    }

};
