


#include "CameraLayer.h"
#include "Ifnity/Event/WindowEvent.h"




IFNITY_NAMESPACE

CameraLayer::CameraLayer(): Layer("CameraLayer"), m_cameraListener(nullptr)
{
	
}



void CameraLayer::OnAttach()
{
	deltaTime = glfwGetTime();
}



void CameraLayer::OnDetach()
{

	deltaTime = 0.0f;
}




void CameraLayer::OnUpdate()
{
	double currentTime = glfwGetTime();
	deltaTime = currentTime - deltaTime;
	m_cameraListener->onUpdateCamera(deltaTime);
	deltaTime = currentTime;
	
}

void CameraLayer::ConnectToEventBusImpl(void* bus)
{

	auto eventBus = static_cast<GLFWEventSource*>(bus);
	if(eventBus)
	{
		
		CONNECT_EVENT_CAMERA(KeyPressed, eventBus, m_cameraListener);
		CONNECT_EVENT_CAMERA(MouseMove, eventBus , m_cameraListener);
		CONNECT_EVENT_CAMERA(MouseClick, eventBus , m_cameraListener);
	}
	else
	{
		IFNITY_LOG(LogApp, ERROR, "The pointer is not type IFNITY::GLFWEventSource*");
	}


}



IFNITY_END_NAMESPACE

