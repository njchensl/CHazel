#pragma once

#define DECLARE_OPAQUE_HANDLE(handle) typedef void* handle;

#define HZ_TRUE 1
#define HZ_FALSE 0

#define HZ_OK 0
#define HZ_UNSUCCESSFUL 1

#ifdef __cplusplus
extern "C" {
#endif

typedef int HzStatus;
typedef unsigned int HZuint;
typedef int HZint;
typedef size_t HZsize;

HzStatus hzSetAllocationFunction(void* (*alloc)(size_t));
HzStatus hzResetAllocationFunction(void);
HzStatus hzSetDeallocationFunction(void (*dealloc)(void*));
HzStatus hzResetDeallocationFunction(void);

HzStatus hzCreateLayers(HZsize num, HZint* layers);
HzStatus hzDeleteLayers(HZsize count, const HZint* layers);
HzStatus hzSetLayerOnAttachFn(HZint layer, void (*fn)());
HzStatus hzSetLayerOnDetachFn(HZint layer, void (*fn)());
HzStatus hzSetLayerOnUpdateFn(HZint layer, void (*fn)(float));

DECLARE_OPAQUE_HANDLE(HZ_APP)

HZ_APP hzGenApplication(void);
void hzSetApplicationCtor(HZ_APP app, void (*fn)(HZ_APP));
void hzSetApplicationDtor(HZ_APP app, void (*fn)(HZ_APP));
void hzCreateApplication(HZ_APP app);
void hzRunApplication(HZ_APP app);
void hzDestroyApplication(HZ_APP app);

void hzAppPushLayer(HZ_APP app, HZint layer);


#ifdef HZ_ENGINE_BUILD
extern
#endif
void ApplicationMain(void);

#ifdef __cplusplus
}
#endif
