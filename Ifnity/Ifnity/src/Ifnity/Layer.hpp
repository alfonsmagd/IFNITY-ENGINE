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
	virtual void OnEvent(Event& e) {};

	template<typename EventBus>
	void ConnectToEventBus(EventBus bus) {};

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}

	inline const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;

};


IFNITY_END_NAMESPACE