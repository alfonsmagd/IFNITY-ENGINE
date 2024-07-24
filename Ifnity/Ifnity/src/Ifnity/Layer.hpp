#pragma once 

#include "pch.h"
#include "Ifnity\Event\EventBus.h"
#include "Ifnity\Event\Event.h"

//This type of layer is based of Hazel Engine Layer. 

IFNITY_NAMESPACE

class IFNITY_API Layer
{
	
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();
	
	virtual void ConnectToEventBusImpl(void* bus) = 0;
	template<typename EventBus>
	void ConnectToEventBus(EventBus bus) {
		if constexpr(std::is_pointer_v<std::decay_t<EventBus>>)
		{
			// If bus is a pointer, we can directly pass it
			ConnectToEventBusImpl(static_cast<void*>(bus));
		}
		else
		{
			// If bus is not a pointer we need to pass the address of the object
			ConnectToEventBusImpl(static_cast<void*>(&bus));
		}
	};

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}

	inline const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;

};


IFNITY_END_NAMESPACE