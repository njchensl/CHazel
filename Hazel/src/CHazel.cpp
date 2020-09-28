#include "hzpch.h"
#include "CHazel.h"


#include <map>
#include <queue>


#include "Hazel/Core/Application.h"
#include "Hazel/Core/Layer.h"

namespace Hazel
{
	bool g_IsDefaultAllocationFunction = true;
	bool g_IsDefaultDeallocationFunction = true;
	void* (*g_AllocationFunction)(size_t);
	void (*g_DeallocationFunction)(void*);

	template <typename T, typename... Args>
	T* Create(Args&&... args)
	{
		if (g_IsDefaultAllocationFunction)
		{
			return new T(std::forward(args)...);
		}
		T* ptr = static_cast<T*>(g_AllocationFunction(sizeof(T)));
		return new(ptr) T(std::forward(args)...);
	}

	template <typename T>
	void DestroyPrimitive(T* ptr)
	{
		if (g_IsDefaultDeallocationFunction)
		{
			delete ptr;
		}
		else
		{
			g_DeallocationFunction(ptr);
		}
	}

	template <typename T>
	void Destroy(T* ptr)
	{
		if (g_IsDefaultDeallocationFunction)
		{
			delete ptr;
		}
		else
		{
			ptr->~T();
			g_DeallocationFunction(ptr);
		}
	}

	template <typename T>
	void DestroyPrimitiveArray(T arr[])
	{
		if (g_IsDefaultDeallocationFunction)
		{
			delete[] arr;
		}
		else
		{
			g_DeallocationFunction(arr);
		}
	}

	// make sure that the T here is the actual type of the elements of the array!!!!!!!!!!!!
	template <typename T>
	void DestroyArray(T arr[], const size_t count)
	{
		if (g_IsDefaultDeallocationFunction)
		{
			delete[] arr;
		}
		else
		{
			for (size_t i = 0; i < count; i++)
			{
				arr[i].~T();
			}
			g_DeallocationFunction(arr);
		}
	}

	// default empty function
	template <typename R, typename... Args>
	R EmptyFn(Args ... args)
	{
		// do nothing
		return R();
	}

	template <>
	void EmptyFn<void>()
	{
		// do nothing
	}

	class CLayer final : public Layer
	{
	public:
		virtual void OnAttach() override
		{
			OnAttachFn();
		}

		virtual void OnDetach() override
		{
			OnDetachFn();
		}

		virtual void OnUpdate(Timestep ts) override
		{
			OnUpdateFn(ts);
		}

		virtual void OnImGuiRender() override
		{
			OnImGuiRenderFn();
		}

		virtual void OnEvent(Event& event) override
		{
		}

		void (*OnAttachFn)() = EmptyFn;
		void (*OnDetachFn)() = EmptyFn;
		void (*OnUpdateFn)(float) = EmptyFn;
		void (*OnImGuiRenderFn)() = EmptyFn;
		HZuint ID;
	};

	class LayerManager final
	{
	public:
		static HZint InternalGenLayer()
		{
			HZint id = NewID();
			m_Layers[id] = CLayer();
			return id;
		}

		static HzStatus InternalDeleteLayer(HZint toDelete)
		{
			if (m_Layers.find(toDelete) == m_Layers.end())
			{
				return HZ_UNSUCCESSFUL;
			}
			m_Layers.erase(toDelete);
			m_IDQueue.push(toDelete); // return the ID to the pool
			return HZ_OK;
		}

		static std::map<HZint, CLayer>& GetLayers()
		{
			return m_Layers;
		}

	private:
		static HZint NewID()
		{
			if (m_IDQueue.empty())
			{
				return m_Counter++;
			}
			HZint newID = m_IDQueue.front();
			m_IDQueue.pop();
			return newID;
		}

		inline static std::queue<HZint> m_IDQueue;
		inline static HZint m_Counter = 0;
		inline static std::map<HZint, CLayer> m_Layers;
	};

	class CApplication final : public Application
	{
	public:
		CApplication() : Application()
		{
			Ctor(this);
		}

		virtual ~CApplication() override
		{
			Dtor(this);
		}

		void (*Ctor)(HZ_APP) = EmptyFn;
		void (*Dtor)(HZ_APP) = EmptyFn;

		static CApplication* InternalGenCApplication()
		{
			auto* ptr = static_cast<CApplication*>(malloc(sizeof(CApplication)));
			HZ_CORE_ASSERT(ptr != nullptr, "Cannot allocate mem for application!");
			memset(static_cast<void*>(ptr), 0, sizeof(CApplication));
			return ptr;
		}

		static void InternalDestroyApplication(CApplication* ptr)
		{
			ptr->~CApplication();
			free(ptr);
		}
	};
}

using namespace Hazel;

HzStatus hzSetAllocationFunction(void*(* alloc)(size_t))
{
	g_AllocationFunction = alloc;
	g_IsDefaultAllocationFunction = false;
	return HZ_OK;
}

HzStatus hzResetAllocationFunction()
{
	if (g_IsDefaultAllocationFunction)
	{
		return HZ_UNSUCCESSFUL;
	}
	g_IsDefaultAllocationFunction = true;
	return HZ_OK;
}

HzStatus hzSetDeallocationFunction(void (*dealloc)(void*))
{
	g_DeallocationFunction = dealloc;
	g_IsDefaultDeallocationFunction = false;
	return HZ_OK;
}

HzStatus hzResetDeallocationFunction()
{
	if (g_IsDefaultDeallocationFunction)
	{
		return HZ_UNSUCCESSFUL;
	}
	g_IsDefaultDeallocationFunction = true;
	return HZ_OK;
}


HzStatus hzCreateLayers(const HZsize num, HZint* layers)
{
	for (int i = 0; i < num; i++)
	{
		layers[i] = LayerManager::InternalGenLayer();
	}
	return HZ_OK;
}

HzStatus hzDeleteLayers(const HZsize count, const HZint* layers)
{
	HzStatus status = HZ_OK;
	for (int i = 0; i < count; ++i)
	{
		if (LayerManager::InternalDeleteLayer(layers[i]) != HZ_OK)
		{
			status = HZ_UNSUCCESSFUL;
		}
	}
	return status;
}

HzStatus hzSetLayerOnAttachFn(const HZint layer, void (* fn)())
{
	if (LayerManager::GetLayers().find(layer) == LayerManager::GetLayers().end())
	{
		return HZ_UNSUCCESSFUL;
	}
	LayerManager::GetLayers()[layer].OnAttachFn = fn;
}

HzStatus hzSetLayerOnDetachFn(const HZint layer, void (* fn)())
{
	if (LayerManager::GetLayers().find(layer) == LayerManager::GetLayers().end())
	{
		return HZ_UNSUCCESSFUL;
	}
	LayerManager::GetLayers()[layer].OnDetachFn = fn;
}

HzStatus hzSetLayerOnUpdateFn(const HZint layer, void (* fn)(float))
{
	if (LayerManager::GetLayers().find(layer) == LayerManager::GetLayers().end())
	{
		return HZ_UNSUCCESSFUL;
	}
	LayerManager::GetLayers()[layer].OnUpdateFn = fn;
}

HZ_APP hzGenApplication()
{
	return CApplication::InternalGenCApplication();
}

void hzSetApplicationCtor(HZ_APP app, void (*fn)(HZ_APP))
{
	static_cast<CApplication*>(app)->Ctor = fn;
}

void hzSetApplicationDtor(HZ_APP app, void (*fn)(HZ_APP))
{
	static_cast<CApplication*>(app)->Dtor = fn;
}

void hzCreateApplication(HZ_APP app)
{
	auto* application = static_cast<CApplication*>(app);
	auto* ctor = application->Ctor;
	auto* dtor = application->Dtor;
	new(application) CApplication;
	application->Ctor = ctor;
	application->Dtor = dtor;
	ctor(application);
	Application::s_Instance = application;
}

void hzRunApplication(HZ_APP app)
{
	static_cast<CApplication*>(app)->Run();
}

void hzDestroyApplication(HZ_APP app)
{
	CApplication::InternalDestroyApplication(static_cast<CApplication*>(app));
}

void hzAppPushLayer(HZ_APP app, HZint layer)
{
	static_cast<CApplication*>(app)->PushLayer(&LayerManager::GetLayers()[layer]);
}
