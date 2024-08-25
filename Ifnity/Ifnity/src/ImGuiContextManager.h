// ImGuiContextManager.h

#pragma once

#include "Ifnity/Core.h"
#include <imgui.h>



IFNITY_API void InitializeImGui();
IFNITY_API ImGuiContext* GetImGuiContext();
IFNITY_API void DestroyImGuiContext();


