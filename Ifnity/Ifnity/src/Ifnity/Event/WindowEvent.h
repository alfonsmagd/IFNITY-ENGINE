#pragma once
#include "pch.h"


#include "Event.h"
#include "EventBus.h"
#include <GLFW/glfw3.h>


namespace IFNITY {

	using WindowGroupSourceEvent   = events::EventTemplateSource<WINDOW_EVENT_GROUP>;
	using WindowGroupListenerEvent = events::EventTemplateListener<WINDOW_EVENT_GROUP>;


	class GLFWEventSource : public WindowGroupSourceEvent
	{
	public:
		// Aquí podrías tener métodos para disparar los eventos específicos
		void triggerWindowResize(int width, int height)
		{
			dispatchEvent(WindowResize(width, height));
		}

		void triggerWindowClose()
		{
			dispatchEvent(WindowClose());
		}
	};

	class GLFWEventListener: public WindowGroupListenerEvent
	{
	public:
		void onEventReceived(const WindowResize& event)
		{
			IFNITY_LOG(LogCore, TRACE, event.ToString());
		}

		void onEventReceived(const WindowClose& event)
		{
			IFNITY_LOG(LogCore, TRACE, event.ToString());
			m_runing = false;
		}
		bool getRunning() { return m_runing; }
	private:
		bool m_runing = true;
	};
  
}
