#pragma once



#include "Event.h"
#include <GLFW/glfw3.h>


namespace IFNITY {

	class IFNITY_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

	class IFNITY_API WindowCloseEvent: public Event
	{
			
	public:
		WindowCloseEvent() = default;

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowOpengl Close";
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowClose)
	    EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
