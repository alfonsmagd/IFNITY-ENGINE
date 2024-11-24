

#pragma once
#include "pch.h"
#include "Ifnity/Layer.hpp"
#include "CameraEvent.h"


IFNITY_NAMESPACE



class IFNITY_API CameraLayer: public Layer
{

public:
	CameraLayer();
	CameraLayer(EventCameraListener* cameraListener): Layer("CameraLayer"), m_cameraListener(cameraListener) {}


	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void ConnectToEventBusImpl(void* bus) override;


private:
	EventCameraListener* m_cameraListener;
	double deltaTime = 0.0f;
	


};

IFNITY_END_NAMESPACE








