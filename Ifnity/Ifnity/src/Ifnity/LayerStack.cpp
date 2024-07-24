#include "LayerStack.hpp"

IFNITY_NAMESPACE

LayerStack::~LayerStack()
{	

	for(Layer* layer : m_Layers)
	{
		delete layer;
	}

}



void LayerStack::PushLayer(Layer* layer)
{
	//Insert layer at the layer insert iterator. 
	m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
	
}

void LayerStack::PushOverlay(Layer* overlay)
{

	//Insert overlay at the end of the layer stack. 
	m_Layers.emplace_back(overlay);
}

void LayerStack::PopLayer(Layer* layer)
{
	//Find the layer in the layer stack. 
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);

	//If the layer is found, erase it. 
	if(it != m_Layers.end())
	{
		m_Layers.erase(it);
		m_LayerInsert--;
	}
}

void LayerStack::PopOverlay(Layer* overlay)
{
	//Find the overlay in the layer stack. 
	auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);

	//If the overlay is found, erase it. 
	if(it != m_Layers.end())
	{
		m_Layers.erase(it);
	}
}



IFNITY_END_NAMESPACE

