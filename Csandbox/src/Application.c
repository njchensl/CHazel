#include <CHazel.h>
#include <stdio.h>

int g_UseCAPI = 1;

void OnUpdate(float ts)
{
	printf("In update loop\n");
}

HZint layer;

void appCtor(HZ_APP app)
{
	printf("Ctor\n");
	hzAppPushLayer(app, layer);
}

void appDtor(HZ_APP app)
{
	printf("Dtor");
}

void ApplicationMain(void)
{
	hzCreateLayers(1, &layer);
	hzSetLayerOnUpdateFn(layer, OnUpdate);

	HZ_APP app = hzGenApplication();
	hzSetApplicationCtor(app, appCtor);
	hzSetApplicationDtor(app, appDtor);
	hzCreateApplication(app);
	hzRunApplication(app);
	hzDestroyApplication(app);
	
}
