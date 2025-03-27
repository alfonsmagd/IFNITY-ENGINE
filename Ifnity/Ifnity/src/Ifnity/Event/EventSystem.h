#include "pch.h"

#include "EventMacro.h"

IFNITY_NAMESPACE

namespace events
{

    /// The EventListener class is an abstract template class designed to be
    /// used in all the classes that listen to events.

    template <typename Event>
    class EventListener
    {
    public:
        virtual ~EventListener()
        {}

        /// This method is called every time the listener receives an event.
        virtual void onEventReceived(const Event& event) = 0;
    };

    template <class Event>
    class EventSource
    {
    public:
        virtual ~EventSource()
        {}

        /// Register the eventListener to this EventSource to receive all the
        /// events dispatched from it.
        void addEventListener(EventListener<Event>& eventListener) const
        {
            theEventListeners.push_back(&eventListener);
        }

        /// Unregister the eventListener from this EventSource.
        void removeEventListener(EventListener<Event>& eventListener) const
        {
            theEventListeners.erase(std::remove(theEventListeners.begin(), theEventListeners.end(), &eventListener), theEventListeners.end());
        }

        /// Dispatches the given event to all the listeners which have been
        /// registered for the event.
        void dispatchEvent(const Event& event) const
        {
            for(const auto& listener : theEventListeners)
            {
                listener->onEventReceived(event);
            }
        }



    protected:
        EventSource()
        {}

        EventSource(const EventSource& eventSource)
        {

        }

    private:
        typedef std::vector<EventListener<Event>* > EventListenerList;

        mutable EventListenerList theEventListeners;

    };

    /// The connect function template is a syntax sugar to register an event
    /// listener to an event source.
 
    template <class Event>
    void connect(const EventSource<Event>& eventSource, EventListener<Event>& eventlistener)
    {
        eventSource.addEventListener(eventlistener);
    }

    // The connect function template is a syntax sugar to unregister an event
    // listener from an event source.
    template <class Event>
    void disconnect(const EventSource<Event>& eventSource, EventListener<Event>& eventlistener)
    {
        eventSource.removeEventListener(eventlistener);
    }

    // namespace events


       // The EventRepeater class is an abstract template class designed to listen
       // to events and automatically dispatch them.
    template <class Event>
    class EventRepeater:
        public EventListener<Event>,
        public EventSource<Event>
    {
    public:
        void onEventReceived(const Event& event)
        {
            EventSource<Event>::dispatchEvent(event);
        }
    };

} // namespace events

IFNITY_END_NAMESPACE