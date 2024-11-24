#pragma once


//Silence the warning deprecated associate with iterators
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include <xutility>

#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <utility>
#include <sstream>
#include <ifnity/Log.h>
#include "../vendor/imgui/imgui.h"
#include <implot.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Ifnity/Graphics/ifrhi.h>

#ifdef _WIN64
	#include  <Windows.h>
#endif


