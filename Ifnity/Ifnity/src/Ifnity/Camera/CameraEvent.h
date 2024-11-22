#pragma once
#include "pch.h"
#include "Ifnity/Event/Event.h"
#include "Ifnity/Event/EventBus.h"
#include "Camera.hpp"
#include <GLFW\glfw3.h>


namespace IFNITY
{


	using CameraGroupListenerEvent = events::EventTemplateListener<CAMERA_EVENT_GROUP>;


	class IFNITY_API EventCameraListener: public CameraGroupListenerEvent
	{

	public:

		//Constructor to set the camera positioner , I dont want copy constructor.
		EventCameraListener(CameraPositioner_FirstPerson* cameraPositioner)
			:m_cameraPositioner(cameraPositioner)
		{}

		//No copy constructor.
		EventCameraListener(const EventCameraListener&) = delete;
		//No move constructor.
		EventCameraListener(EventCameraListener&&) = delete;
		//No copy assignment.
		EventCameraListener& operator=(const EventCameraListener&) = delete;
		//No move assignment.
		EventCameraListener& operator=(EventCameraListener&&) = delete;

		//Destructor.
		~EventCameraListener() = default;

		glm::vec2 getMousePos() const { return mouseState.pos; }
		bool isMousePressed() const   { return mouseState.pressedLeft; }

		void onEventReceived(const MouseMove& event) override
		{
			mouseState.pos.x = static_cast<float>(event.getX());
			mouseState.pos.y = static_cast<float>(event.getY());
			IFNITY_LOG(LogCore, TRACE, event.ToString() + "camera");
		}

		void onEventReceived(const MouseClick& event) override
		{
			mouseState.pressedLeft = event.getButton() == GLFW_MOUSE_BUTTON_LEFT &&
				event.getState() == GLFW_PRESS;
			IFNITY_LOG(LogCore, TRACE, event.ToString() + "camera");
		}

		void onEventReceived(const KeyPressed& event)	override
		{
			const bool pressed = event.getAction() != GLFW_RELEASE;
			const int key = event.getKey();

			
			if(key == GLFW_KEY_W)
				m_cameraPositioner->movement_.forward_ = pressed;
			if(key == GLFW_KEY_S)
				m_cameraPositioner->movement_.backward_ = pressed;
			if(key == GLFW_KEY_A)
				m_cameraPositioner->movement_.left_ = pressed;
			if(key == GLFW_KEY_D)
				m_cameraPositioner->movement_.right_ = pressed;
			if(key == GLFW_KEY_1)
				m_cameraPositioner->movement_.up_ = pressed;
			if(key == GLFW_KEY_2)
				m_cameraPositioner->movement_.down_ = pressed;
			IFNITY_LOG(LogCore, TRACE, event.ToString() + "camera");
		}

		void onUpdateCamera(double deltaSeconds)
		{
			m_cameraPositioner->update(deltaSeconds, mouseState.pos, mouseState.pressedLeft);
		}


	private:

		
		CameraPositioner_FirstPerson* m_cameraPositioner;


		struct MouseState
		{
			glm::vec2 pos = glm::vec2(0.0f);
			bool pressedLeft = false;
		} mouseState;
		
		///Loggin function Event. 
		template<typename EventType>
		void logEvent(const EventType& event)
		{
			#ifdef _DEBUG
			IFNITY_LOG(LogCore, TRACE, event.ToString() + "Camera Listener Event");
			#endif
		}

	};


	

}
