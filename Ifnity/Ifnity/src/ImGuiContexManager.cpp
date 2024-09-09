
#include "ImGuiContextManager.h"
#include <implot.h>



// Variable estática para almacenar el contexto de ImGui
static ImGuiContext* g_ImGuiContext = nullptr;
static ImPlotContext* g_ImPlotContext = nullptr;
// Función para inicializar ImGui
IFNITY_API void InitializeImGui()
{
    if (g_ImGuiContext == nullptr)
    {
        g_ImGuiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(g_ImGuiContext);
        // Configuración adicional de ImGui si es necesario
    }
	if (g_ImPlotContext == nullptr)
	{
		g_ImPlotContext = ImPlot::CreateContext();
		ImPlot::SetCurrentContext(g_ImPlotContext);
	}
}

// Función para obtener el contexto de ImGui
 IFNITY_API ImGuiContext* GetImGuiContext()
{
    return g_ImGuiContext;

}

 // Function to destroy ImGui context.
 IFNITY_API void DestroyImGuiContext()
 {
     if (g_ImGuiContext != nullptr)
     {
         ImGui::DestroyContext(g_ImGuiContext);
         g_ImGuiContext = nullptr;
     }
	 if (g_ImPlotContext != nullptr)
	 {
		 ImPlot::DestroyContext(g_ImPlotContext);
		 g_ImPlotContext = nullptr;
	 }
 }