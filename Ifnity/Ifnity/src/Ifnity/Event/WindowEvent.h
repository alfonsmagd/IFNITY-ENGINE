#pragma once
#include "pch.h"

#include "Event.h"
#include "EventBus.h"




namespace IFNITY {

	using WindowGroupSourceEvent   = events::EventTemplateSource<WINDOW_EVENT_GROUP>;
	using WindowGroupListenerEvent = events::EventTemplateListener<WINDOW_EVENT_GROUP>;


	class GLFWEventSource : public WindowGroupSourceEvent
	{
	
	public:
		template<typename EventType, typename... Args>
		void triggerEvent(Args&&... args)
		{
			EventType event(std::forward<Args>(args)...);
			dispatchEvent(event);
		}
	};


	/// <summary>
	/// This class is an example of how 
	/// </summary>
	class IFNITY_API GLFWEventListener: public WindowGroupListenerEvent
	{
	public:
		
	
		virtual void onEventReceived(const WindowResize& event) 
		{
			logEvent(event);

		}

		void onEventReceived(const WindowClose& event) 
		{
			logEvent(event);
			m_running = false;
		}

		virtual void onEventReceived(const KeyPressed& event) 
		{
			logEvent(event);
		}

		virtual void onEventReceived(const KeyRelease& event) 
		{
			logEvent(event);
		}

		virtual void onEventReceived(const MouseMove& event) 
		{
			logEvent(event);
		}

		virtual void onEventReceived(const ScrollMouseMove& event)
		{
			logEvent(event);
		}

		virtual void onEventReceived(const MouseClick& event)
		{
			logEvent(event);
		}

	
		
		bool getRunning() { return m_running; }
	private:
		bool m_running = true;
		
	
		///Loggin function Event. 
		template<typename EventType>
		void logEvent(const EventType& event)
		{
			#ifdef _DEBUG
				IFNITY_LOG(LogCore, TRACE, event.ToString());
			#endif
		}
	};
  
}
