#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"

extern "C" int g_UseCAPI = 0;

namespace Hazel {

	class Hazelnut : public Application
	{
	public:
		Hazelnut()
			: Application("Hazelnut")
		{
			PushLayer(new EditorLayer());
		}

		~Hazelnut()
		{
		}
	};

	Application* CreateApplication()
	{
		return new Hazelnut();
	}

}
