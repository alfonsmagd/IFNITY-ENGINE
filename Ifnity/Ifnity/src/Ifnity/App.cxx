#include "App.h"

IFNITY_NAMESPACE

// Implementación de la función template
template<typename EvenType>
void App::ConnectEvent() const
{
	events::connect<EvenType>(*m_RenderDevice->GetGLFWEventSource(), *m_GLFWEventListener);
}

IFNITY_END_NAMESPACE