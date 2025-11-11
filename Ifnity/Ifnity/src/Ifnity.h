#pragma once

//IFNITY ENGINE USES

#include "Ifnity/Utils/VFS.hpp"
#include "Ifnity/App.h"
#include "Ifnity/Layer.hpp"
#include "Ifnity/Layers/ImguiLayer.h"
#include "Ifnity/Layers/NVML_Layer.hpp"
#include "Ifnity/Layers/ExampleLayer.h"
#include "Ifnity/Camera/CameraLayer.h"
#include "ShaderBuilding\ShaderBuilder.hpp"
#include "BenchMark/FpsCounter.hpp"



//Maths
#include "Ifnity/Maths/math.h"

//Models
#include "Ifnity/Graphics/Interfaces/IMeshDataBuilder.hpp"
#include "Ifnity/Models/GeometicModels.hpp"


//Materials
#include "Ifnity/Scene/Material.h"

//Scene
#include "Ifnity/Scene/Scene.h"

//Utils Vulkan to test Vulkan API
#ifdef IFNITY_VULKAN_API
#include "Platform\Windows\UtilsVulkan.h"
#endif


//Renderers
#include "Platform/Renderers/Simple.hpp"

//wOMDPW
//Entry Point this include have put at the end of the file.
#include "Ifnity/EntryPoint.h"



