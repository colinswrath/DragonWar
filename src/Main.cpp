#include "Hooks.h"
#include "FormLoader.h"
#include "Settings.h"
#include "Events.h"

void InitLogger()
{
    auto path{ SKSE::log::log_directory() };
    if (!path)
        stl::report_and_fail("Unable to lookup SKSE logs directory.");
    *path /= SKSE::PluginDeclaration::GetSingleton()->GetName();
    *path += L".log";

    std::shared_ptr<spdlog::logger> log;
    if (IsDebuggerPresent())
        log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
    else
        log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));

    log->set_level(spdlog::level::level_enum::info);
    log->flush_on(spdlog::level::level_enum::trace);

    set_default_logger(std::move(log));

    spdlog::set_pattern("[%T.%e UTC%z] [%L] [%=5t] %v");
}

void InitListener(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
        FormLoader::GetSingleton()->LoadAllForms();
        Settings::GetSingleton()->LoadSettings();
        Hooks::Install();
        Events::OnCombatEventHandler::Register();
		break;
	}
}

extern "C" DLLEXPORT constexpr auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v{};
    v.PluginVersion(REL::Version{ Version::MAJOR, Version::MINOR, Version::PATCH, 0 });
    v.PluginName("DragonWar"sv);
    v.AuthorName("colinswrath and ThirdEyeSqueegee"sv);
    v.UsesAddressLibrary(true);
    v.HasNoStructUse(true);
    v.UsesStructsPost629(false);
    return v;
}();

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitLogger();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto version{ plugin->GetVersion() };
    auto runtimcompat = plugin->GetRuntimeCompatibility();

    logger::info("{} {} loading...", plugin->GetName(), version);

    #ifdef SKYRIM_SUPPORT_AE
        logger::info("Post 1130 build is active.");
    #endif // SKYRIM_SUPPORT_AE

    SKSE::Init(skse);

    SKSE::AllocTrampoline(42);

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener(InitListener)) {
        return false;
    }

    logger::info("DragonWar loaded successfully");
    return true;
}
