
#include "ImGuiContextManager.h"
#include <implot.h>



// Variable est�tica para almacenar el contexto de ImGui
static ImGuiContext* g_ImGuiContext = nullptr;
static ImPlotContext* g_ImPlotContext = nullptr;
// Funci�n para inicializar ImGui
IFNITY_API void InitializeImGui()
{
    if (g_ImGuiContext == nullptr)
    {
        g_ImGuiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(g_ImGuiContext);
        // Configuraci�n adicional de ImGui si es necesario
    }
	if (g_ImPlotContext == nullptr)
	{
		g_ImPlotContext = ImPlot::CreateContext();
		ImPlot::SetCurrentContext(g_ImPlotContext);
	}
}

// Funci�n para obtener el contexto de ImGui
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