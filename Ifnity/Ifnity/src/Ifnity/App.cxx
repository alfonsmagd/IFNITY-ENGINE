#include "App.h"

IFNITY_NAMESPACE

// Implementaci�n de la funci�n template
template<typename EvenType>
void App::ConnectEvent() const
{
	events::connect<EvenType>(*m_RenderDevice->GetGLFWEventSource(), *m_GLFWEventListener);
}

IFNITY_END_NAMESPACE