#include "pch.h"

//IFNITY ENGINE USES

#include "Ifnity/App.h"
#include "Ifnity/Layer.hpp"
#include "Ifnity/Event/WindowEvent.h"
#include "Ifnity/Layers/ImguiLayer.h"
#include "Ifnity/Log.h"


//wOMDPW
#include "BenchMark/NvmlMonitor.hpp"



#include <imgui.h>
#include <imgui_internal.h>
#include <implot.h>

#define NVP_SUPPORTS_NVML   //TODO:: Propagate this to the CMakeLists.txt in source files. 
#define SAMPLING_INTERVAL 100
#define SAMPLING_NUM 100
//Get the time to get when use elapsed(); 
struct Stopwatch
{
	Stopwatch() { reset(); }
	void   reset() { startTime = std::chrono::steady_clock::now(); }
	double elapsed()
	{
		return std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() * 1000.;
	}
	std::chrono::time_point<std::chrono::steady_clock> startTime;
};


inline int metricFormatter(double value, char* buff, int size, void* data)
{
	const char* unit = (const char*)data;
	static double      s_value[] = { 1000000000, 1000000, 1000, 1, 0.001, 0.000001, 0.000000001 };
	static const char* s_prefix[] = { "G", "M", "k", "", "m", "u", "n" };
	if (value == 0)
	{
		return snprintf(buff, size, "0 %s", unit);
	}
	for (int i = 0; i < 7; ++i)
	{
		if (fabs(value) >= s_value[i])
		{
			return snprintf(buff, size, "%g %s%s", value / s_value[i], s_prefix[i], unit);
		}
	}
	return snprintf(buff, size, "%g %s%s", value / s_value[6], s_prefix[6], unit);
}


// utility structure for averaging values
template <typename T>
struct AverageCircularBuffer
{
	int            offset = 0;
	T              totValue = 0;
	std::vector<T> data;
	AverageCircularBuffer(int max_size = 100) { data.reserve(max_size); }
	void addValue(T x)
	{
		if (data.size() < data.capacity())
		{
			data.push_back(x);
			totValue += x;
		}
		else
		{
			totValue -= data[offset];
			totValue += x;
			data[offset] = x;
			offset = (offset + 1) % data.capacity();
		}
	}

	T average() { return totValue / data.size(); }
};


class NVML_Monitor : public IFNITY::Layer
{
public:
	 explicit NVML_Monitor() : Layer("NVML_Monitor")
	{

#if defined(NVP_SUPPORTS_NVML)
		m_nvmlMonitor = std::make_unique<NvmlMonitor>(SAMPLING_INTERVAL, SAMPLING_NUM);
#endif
	

	}
	~NVML_Monitor() = default;

	virtual void ConnectToEventBusImpl(void* bus) {};

	void OnAttach() override
	{
		addSettingsHandler();
	}


private:
	void addSettingsHandler()
	{
		if (ImGui::GetCurrentContext() == nullptr)
		{
			IFNITY_LOG(LogApp, ERROR, "IMGUI CONTEXT NOT INITIALIZED " , __FUNCTION__);
			return;
		}
		ImGuiSettingsHandler iniHandler{};
		iniHandler.TypeName = "NVML_Monitor";
		iniHandler.TypeHash = ImHashStr("NVML_Monitor");
		iniHandler.ClearAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler*) {};
		iniHandler.ApplyAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler*) {};
		iniHandler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler*, const char* name) -> void* { return (void*)1; };
		iniHandler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler* handler, void* entry, const char* line) {
			NVML_Monitor* s = (NVML_Monitor*)handler->UserData;
			int          x;
			if (sscanf(line, "ShowLoader=%d", &x) == 1)
			{
				s->m_showWindow = (x == 1);
			}
			};
		iniHandler.WriteAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
			NVML_Monitor* s = (NVML_Monitor*)handler->UserData;
			buf->appendf("[%s][State]\n", handler->TypeName);
			buf->appendf("ShowLoader=%d\n", s->m_showWindow ? 1 : 0);
			buf->appendf("\n");
			};
		iniHandler.UserData = this;
		ImGui::AddSettingsHandler(&iniHandler);
	}
private:

	bool           m_showWindow{ false };
	bool           m_throttleDetected{ false };
	uint64_t       m_lastThrottleReason{ 0ull };
	Stopwatch m_throttleCooldownTimer;

	uint32_t m_selectedMemClock{ 0u };
	uint32_t m_selectedGraphicsClock{ 0u };


#if defined(NVP_SUPPORTS_NVML)
	std::unique_ptr<NvmlMonitor> m_nvmlMonitor;
	AverageCircularBuffer<float> m_avgCpu = { SAMPLING_NUM };
#endif

};
