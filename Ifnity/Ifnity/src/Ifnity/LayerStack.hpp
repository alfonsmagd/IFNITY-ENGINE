#pragma once

#include "Layer.hpp"


IFNITY_NAMESPACE

class  IFNITY_API LayerStack
{

public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);
	void PopLayer(Layer* layer);
	void PopOverlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }

private:
	std::vector<Layer*> m_Layers;
	std::vector<Layer*>::iterator m_LayerInsert;
	unsigned int m_LayerInsertIndex = 0;
};


IFNITY_END_NAMESPACE