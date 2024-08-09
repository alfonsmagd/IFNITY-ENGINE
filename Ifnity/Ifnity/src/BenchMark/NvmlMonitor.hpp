#pragma once

#include "pch.h"
/* 
This class is bassed on the NvmlMonitor class from the NVIDIA NVML example 
Capture the GPU load and memory for all GPUs on the system.

Usage:
- There should be only one instance of NvmlMonitor
- call refresh() in each frame. It will not pull more measurement that the interval(ms)
- isValid() : return if it can be used
- nbGpu()   : return the number of GPU in the computer
- getGpuInfo()     : static info about the GPU
- getDeviceMemory() : memory consumption info
- getDeviceUtilization() : GPU and memory utilization
- getDevicePerformanceState() : clock speeds and throttle reasons
- getDevicePowerState() : power, temperature and fan speed

Measurements:
- Uses a cycle buffer.
- Offset is the last measurement



*/

/// <summary>
/// Locator pattern for maintenance of the NVML library and usage with Logger or IMGUI 
/// </summary>
class NvmlMonitor;
class IFNITY_API IDisplayBenchMark
{
public:
    virtual void display(const NvmlMonitor& monitor) const  = 0;
    //Destructor 
    virtual ~IDisplayBenchMark() {}

};

class IFNITY_API NvmlMonitor
{
public:
    NvmlMonitor(uint32_t interval = 100, uint32_t limit = 100);
    ~NvmlMonitor();

    template <typename T>
    struct NVMLField
    {
        T    data;
        bool isSupported;

        operator T& () { return data; }
        T& get() { return data; }
        const T& get() const { return data; }

        T& operator=(const T& rhs)
        {
            data = rhs;
            return data;
        }
    };

	/// Generate IRefreshable interface for the class
	struct IRefreshable
	{
		virtual void refresh(void* device, uint32_t offset = 0) = 0;
	};

    /// <summary>
    /// Static device information 
    /// </summary>
	struct DeviceInfo
    {
        NVMLField<std::string> currentDriverModel;
        NVMLField<std::string> pendingDriverModel;

        NVMLField<uint32_t>    boardId;
        NVMLField<std::string> partNumber;
        NVMLField<std::string> brand;
        // Ordered list of bridge chips, each with a type and firmware version strings
        NVMLField<std::vector<std::pair<std::string, std::string>>> bridgeHierarchy;
        NVMLField<uint64_t>                                         cpuAffinity;
        NVMLField<std::string>                                      computeMode;
        NVMLField<int32_t>                                          computeCapabilityMajor;
        NVMLField<int32_t>                                          computeCapabilityMinor;
        NVMLField<uint32_t>                                         pcieLinkGen;
        NVMLField<uint32_t>                                         pcieLinkWidth;

        NVMLField<uint32_t> clockDefaultGraphics;
        NVMLField<uint32_t> clockDefaultSM;
        NVMLField<uint32_t> clockDefaultMem;
        NVMLField<uint32_t> clockDefaultVideo;

        NVMLField<uint32_t> clockMaxGraphics;
        NVMLField<uint32_t> clockMaxSM;
        NVMLField<uint32_t> clockMaxMem;
        NVMLField<uint32_t> clockMaxVideo;

        NVMLField<uint32_t> clockBoostGraphics;
        NVMLField<uint32_t> clockBoostSM;
        NVMLField<uint32_t> clockBoostMem;
        NVMLField<uint32_t> clockBoostVideo;


        NVMLField<bool> currentEccMode;
        NVMLField<bool> pendingEccMode;

        NVMLField<uint32_t>    encoderCapacityH264;
        NVMLField<uint32_t>    encoderCapacityHEVC;
        NVMLField<std::string> infoROMImageVersion;
        NVMLField<std::string> infoROMOEMVersion;
        NVMLField<std::string> infoROMECCVersion;
        NVMLField<std::string> infoROMPowerVersion;
        NVMLField<uint64_t>    supportedClocksThrottleReasons;
        NVMLField<std::string> vbiosVersion;
        NVMLField<uint32_t>    maxLinkGen;
        NVMLField<uint32_t>    maxLinkWidth;
        NVMLField<uint32_t>    minorNumber;
        NVMLField<uint32_t>    multiGpuBool;
        NVMLField<std::string> deviceName;


        NVMLField<uint32_t> tempThresholdShutdown;
        NVMLField<uint32_t> tempThresholdHWSlowdown;
        NVMLField<uint32_t> tempThresholdSWSlowdown;
        NVMLField<uint32_t> tempThresholdDropBelowBaseClock;

        NVMLField<uint32_t> powerLimit;

        NVMLField<std::vector<uint32_t>>                     supportedMemoryClocks;
        NVMLField<std::map<uint32_t, std::vector<uint32_t>>> supportedGraphicsClocks;


        void refresh(void* device, uint32_t offset = 0);
    };
   
    /// <summary>
    /// Device Memory Usage. 
    /// </summary>
	struct DeviceMemory: public IRefreshable
    {
        NVMLField<uint64_t>              bar1Total;
        NVMLField<std::vector<uint64_t>> bar1Used;
        NVMLField<std::vector<uint64_t>> bar1Free;

        NVMLField<uint64_t>              memoryTotal;
        NVMLField<std::vector<uint64_t>> memoryUsed;
        NVMLField<std::vector<uint64_t>> memoryFree;

        void init(uint32_t maxElements);
        void refresh(void* device, uint32_t offset);
    };


    // Device performance state: clocks and throttling
	struct DevicePerformanceState: public IRefreshable
    {
        NVMLField<std::vector<uint32_t>> clockGraphics;
        NVMLField<std::vector<uint32_t>> clockSM;
        NVMLField<std::vector<uint32_t>> clockMem;
        NVMLField<std::vector<uint32_t>> clockVideo;
        NVMLField<std::vector<uint64_t>> throttleReasons;

        void                            init(uint32_t maxElements);
        void                            refresh(void* device, uint32_t offset);
        static std::vector<std::string> getThrottleReasonStrings(uint64_t reason);

        static const std::vector<uint64_t>& getAllThrottleReasonList();
    };

    // Device utilization ratios
	struct DeviceUtilization: public IRefreshable
    {
        NVMLField<std::vector<uint32_t>> gpuUtilization;
        NVMLField<std::vector<uint32_t>> memUtilization;
        NVMLField<std::vector<uint32_t>> computeProcesses;
        NVMLField<std::vector<uint32_t>> graphicsProcesses;

        void init(uint32_t maxElements);
        void refresh(void* device, uint32_t offset);
    };

    // Device power and temperature
	struct DevicePowerState: public IRefreshable
    {
        NVMLField<std::vector<uint32_t>> power;
        NVMLField<std::vector<uint32_t>> temperature;
        NVMLField<std::vector<uint32_t>> fanSpeed;

        void init(uint32_t maxElements);
        void refresh(void* device, uint32_t offset);
    };

    // Other information
    struct SysInfo
    {
        std::vector<float> cpu;  // Load measurement [0, 100]
        std::string        driverVersion;
    };

	void						  setDisplay(IDisplayBenchMark* display) { m_display = display; }
    void                          refresh();  // Take measurement
    bool                          isValid()                              { return m_valid; }
    uint32_t                      getGpuCount() const                    { return m_physicalGpuCount; }
    const DeviceInfo& getDeviceInfo(int gpu)const                        { return m_deviceInfo[ gpu ]; }
    const DeviceMemory& getDeviceMemory(int gpu)const                    { return m_deviceMemory[ gpu ]; }
    const DeviceUtilization& getDeviceUtilization(int gpu)const          { return m_deviceUtilization[ gpu ]; }
    const DevicePerformanceState& getDevicePerformanceState(int gpu)const{ return m_devicePerformanceState[ gpu ]; }
    const DevicePowerState& getDevicePowerState(int gpu)const            { return m_devicePowerState[ gpu ]; }
    const SysInfo& getSysInfo()                const                     { return m_sysInfo; }
    int  getOffset()                                                     { return m_offset; }
	void display()                                                       { if(m_display) m_display->display(*this); }   

    private:
	void addRefreshable(IRefreshable* refreshable)                  { m_refreshables.push_back(refreshable); }


    private:
		std::vector<IRefreshable*>		    m_refreshables;
        std::vector<DeviceInfo>             m_deviceInfo;
        std::vector<DeviceMemory>           m_deviceMemory;
        std::vector<DeviceUtilization>      m_deviceUtilization;
        std::vector<DevicePerformanceState> m_devicePerformanceState;
        std::vector<DevicePowerState>       m_devicePowerState;
        SysInfo                             m_sysInfo;  // CPU and driver information
        bool                                m_valid = false;
        uint32_t                            m_physicalGpuCount = 0;    // Number of NVIDIA GPU
        uint32_t                            m_offset = 0;    // Index of the most recent cpu load sample
        uint32_t                            m_maxElements = 100;  // Number of max stored measurements
        uint32_t                            m_minInterval = 100;  // Minimum interval lapse

		IDisplayBenchMark* m_display = nullptr;
};


class IFNITY_API LoggerDisplayMonitor: public IDisplayBenchMark
{
public:
    void display(const  NvmlMonitor& monitor) const override 
    {
		std::stringstream logStream;
		// Display the GPU information
		for(unsigned int i = 0; i < monitor.getGpuCount(); i++)
		{
			const auto& deviceInfo = monitor.getDeviceInfo(i);
			const auto& deviceMemory = monitor.getDeviceMemory(i);
			const auto& deviceUtilization = monitor.getDeviceUtilization(i);
			const auto& devicePerformanceState = monitor.getDevicePerformanceState(i);
			const auto& devicePowerState = monitor.getDevicePowerState(i);

			// Display the GPU information
			logStream << "GPU " << i << ": " << deviceInfo.deviceName.get() << std::endl;
			logStream << "  Driver: " << deviceInfo.currentDriverModel.get() << std::endl;
			logStream << "  Compute Capability: " << deviceInfo.computeCapabilityMajor.get() << "." << deviceInfo.computeCapabilityMinor.get() << std::endl;
            logStream << "  Memory: " << deviceMemory.memoryUsed.get()[ 1 ] << " / " << deviceMemory.memoryTotal.get() << std::endl;
            logStream << "  Utilization: " << deviceUtilization.gpuUtilization.get()[ 1 ] << " % GPU, " << deviceUtilization.memUtilization.get()[ 1 ] << " % Memory" << std::endl;
            logStream << "  Performance State: " << devicePerformanceState.clockGraphics.get()[ 1 ] << " MHz GPU, " << devicePerformanceState.clockMem.get()[ 1 ] << " MHz Memory" << std::endl;
            logStream << "  Power: " << devicePowerState.power.get()[ 1 ] << " W, " << devicePowerState.temperature.get()[ 1 ] << " C, " << devicePowerState.fanSpeed.get()[ 1 ] << " % Fan" << std::endl;

            IFNITY_LOG(LogCore, TRACE, logStream.str());
		}
	}

	//Destructor LoggerDisplayMonitor
	virtual ~LoggerDisplayMonitor() {}

};

class IFNITY_API ImguiNVML: public IDisplayBenchMark
{
public:
   
        void display(const NvmlMonitor & monitor) const override
        {
            // Display the GPU information
            for(unsigned int i = 0; i < monitor.getGpuCount(); i++)
            {
                const auto& deviceInfo = monitor.getDeviceInfo(i);
                const auto& deviceMemory = monitor.getDeviceMemory(i);
                const auto& deviceUtilization = monitor.getDeviceUtilization(i);
                const auto& devicePerformanceState = monitor.getDevicePerformanceState(i);
                const auto& devicePowerState = monitor.getDevicePowerState(i);

                // Create a new ImGui window for each GPU
                ImGui::Begin(("GPU " + std::to_string(i)).c_str());

                // Display the GPU information
                ImGui::Text("GPU %d: %s", i, deviceInfo.deviceName.get().c_str());
                ImGui::Text("  Driver: %s", deviceInfo.currentDriverModel.get().c_str());
                ImGui::Text("  Compute Capability: %d.%d", deviceInfo.computeCapabilityMajor.get(), deviceInfo.computeCapabilityMinor.get());
                ImGui::Text("  Memory: %llu / %llu", deviceMemory.memoryUsed.get()[ 1 ], deviceMemory.memoryTotal.get());
                ImGui::Text("  Utilization: %u %% GPU, %u %% Memory", deviceUtilization.gpuUtilization.get()[ 1 ], deviceUtilization.memUtilization.get()[ 1 ]);
                ImGui::Text("  Performance State: %u MHz GPU, %u MHz Memory", devicePerformanceState.clockGraphics.get()[ 1 ], devicePerformanceState.clockMem.get()[ 1 ]);
                ImGui::Text("  Power: %u W, %u C, %u %% Fan", devicePowerState.power.get()[ 1 ], devicePowerState.temperature.get()[ 1 ], devicePowerState.fanSpeed.get()[ 1 ]);

                ImGui::End();
            }
        
    }

    //Destructor LoggerDisplayMonitor
    virtual ~ImguiNVML() {}

};