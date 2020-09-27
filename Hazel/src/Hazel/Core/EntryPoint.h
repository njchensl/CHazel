#pragma once

#include "Hazel/Core/Base.h"

#ifdef HZ_PLATFORM_WINDOWS

extern "C" int g_UseCAPI;

extern Hazel::Application* Hazel::CreateApplication();

extern "C" void ApplicationMain(void);

int main(int argc, char** argv)
{
	Hazel::Log::Init();

	if (g_UseCAPI)
	{
		ApplicationMain();
	}
	else
	{
		HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
		auto app = Hazel::CreateApplication();
		HZ_PROFILE_END_SESSION();

		HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
		app->Run();
		HZ_PROFILE_END_SESSION();

		HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
		delete app;
		HZ_PROFILE_END_SESSION();
	}
}

#endif
