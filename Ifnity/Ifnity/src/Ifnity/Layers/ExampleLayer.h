#pragma once
#include "pch.h"
#include "Ifnity/Layer.hpp"




IFNITY_NAMESPACE

class IFNITY_API Examp : public Layer
{

public:
	inline Examp() : Layer("ExamLayer") {}
	~Examp() {}

	void ConnectToEventBusImpl(void* bus) override{};
	inline void OnAttach() override {};
	inline void OnDetach() override {};
	inline void OnUpdate() override {};

	

};


IFNITY_END_NAMESPACE