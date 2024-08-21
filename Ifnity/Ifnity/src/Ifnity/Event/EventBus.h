#pragma once


#include "EventSystem.h"

IFNITY_NAMESPACE
namespace events {
	/** @brief EventTemplateRepeater is a template class to define
	 *  a multiple event-repeater.
	 *  @tparam Events Variadic arguments to expand events class.
	 *
	 */
	template<class... Events>
	class EventTemplateRepeater
		: public EventRepeater<Events>...
	{
	public:
		using EventSource<Events>::dispatchEvent...;
	};

	/** @brief EventTemplateSource is a template class to define
	 *  a multiple event-source.
	 *  @tparam Events Variadic arguments to expand events class.
	 *

	 */
	template <class... Events>
	class EventTemplateSource
		: public EventSource<Events>...
	{
	public:
		using EventSource<Events>::dispatchEvent...;
	};

	/** @brief EventTemplateListener is a template class to define
	 *  a multiple event-listener.
	 *  @tparam Events Variadic arguments to expand events class.
	 */
	template <class... Events>
	class EventTemplateListener
		: public EventListener<Events>...
	{
	};
}
IFNITY_END_NAMESPACE