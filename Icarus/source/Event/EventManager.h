/*----==== EVENTMANAGER.H ====----
	Author:		Jeffrey Kiah
	Orig.Date:	07/23/2007
	Rev.Date:	05/04/2009

Event System notes:
	The event system is central to the engine, it is used for communication between all subsystems.
	It is designed to operate with two different patterns:
		queue events for asynchronous handling at the client's convenience
	or
		trigger events to listeners immediately (current execution is interrupted by event handlers)
	
	Entries in the event-listener map are auto-registered, while entries in the event-registration
	map must be created via registerEventType(). Events may not be triggered or raised unless their
	event-registration has been created manually. When an event is fired, only valid listeners will
	be notified (if, for example, a script listener registers for a code-only event before the event
	type is actually registered).
Features:
	* Allows custom event types with custom data carried by event
	* Allows for events defined at run time and through script
	* Event Listeners auto-register themselves with the manager when instantiated
	* Event Handlers auto-register with Listeners
	* Event Handlers can be prioritized so events are handled in the correct order, else FIFO
	* Handlers can consume events to prevent further propagation
	* Wildcard listeners see all events, and can handle them via generic or type-specific handlers
	* Events cannot be fired until their type has been registered
	* Events types include code-only, code/script, and script-defined
	* Listeners can be safely registered before corresponding event type is registered (eliminates
		order of creation issues)
--------------------------------*/

#pragma once

#include <string>
#include <list>
#include <hash_map>
#include "EventListener.h"
#include "Event.h"
#include "Utility/Debug.h"

using std::string;
using std::list;
using std::pair;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;
using std::hash_map;

// Forward declarations
class EventSnooper;
template<typename T> class ConcurrentQueue;

typedef ConcurrentQueue<EventPtr>	ThreadSafeEventQueue;

/*=============================================================================
class EventManager
	The central manager for the event system. Has an event queue that keeps
	raised events to be processed each frame. Event registration and listener
	maps are kept separately so that listeners may be registered for events
	types event before they have been registered.
=============================================================================*/
class EventManager {
	private:
		///// DEFINITIONS /////

		typedef pair<EventListener*, uint32_t>		ListenerListValue;	// pairs the listener pointer with priority
		typedef list<ListenerListValue>				ListenerList;		// stores listeners along with their priority
		typedef pair<string, ListenerList>			EventTypeMapValue;	// value pair of the event type map
		typedef hash_map<string, ListenerList>		EventTypeMap;		// hash_map to store lists of event listeners
		typedef pair<EventTypeMap::iterator, bool>	EventTypeMapResult;	// result of inserting elements into the event type map
		typedef pair<string, RegEventPtr>			RegEventMapValue;	// value pair of the event registration map
		typedef hash_map<string, RegEventPtr>		RegEventMap;		// hash_map to store lists of event registrations
		typedef pair<RegEventMap::iterator, bool>	RegEventMapResult;	// result of inserting elements into event registration map
		typedef list<EventPtr>						EventQueue;

		// add a type returned by listeners for consumed vs. not consumed (allowing further notifications of the event)
		// so a high priority listener may choose to consume an event before others are notified

		///// VARIABLES /////

		RegEventMap		m_regEventMap;		// maps registration types to event types
		EventTypeMap	m_eventTypeMap;		// maps listeners to their event types
		EventQueue		m_eventQueue[2];		// double-buffered list of events that have been raised
		uint32_t		m_activeQueue;		// the active event queue is written to while the inactive queue is being processed

		unique_ptr<EventSnooper> m_eventSnooper; // wildcard event listener

		ThreadSafeEventQueue m_threadEventQueue; // thread-safe event queue, used for inter-thread events

		///// FUNCTIONS /////

		/*---------------------------------------------------------------------
			Cleanup for when manager is destroyed or being reset
		---------------------------------------------------------------------*/
		void clearListeners() { m_eventTypeMap.clear(); }

		/*---------------------------------------------------------------------
			Purge event queue (usually done each frame, called by notifyQueued)
		---------------------------------------------------------------------*/
		void clearEventQueue(uint32_t queueToClear)
		{
			_ASSERTE(queueToClear == 0 || queueToClear == 1);
			m_eventQueue[queueToClear].clear();
		}

		/*---------------------------------------------------------------------
			Notifies listeners of a single event raised or triggered
		---------------------------------------------------------------------*/
		void notifyListeners(const EventPtr &ePtr) const;

		// private constructor, use create
		explicit EventManager(unique_ptr<EventSnooper> &es);

	public:
		/*---------------------------------------------------------------------
			Add event to the queue, queue is processed each frame
		---------------------------------------------------------------------*/
		void raise(const EventPtr &ePtr);

		/*---------------------------------------------------------------------
			Add a no-data event to the queue, queue is processed each frame.
			This cannot be used to trigger events where the registered type
			specifies "EventData_NotEmpty".
		---------------------------------------------------------------------*/
		void raise(const string &eventType);

		/*---------------------------------------------------------------------
			Invoke listeners immediately, does not queue the event
		---------------------------------------------------------------------*/
		void trigger(const EventPtr &ePtr);
		
		/*---------------------------------------------------------------------
			Invoke listeners immediately, does not queue the no-data event.
			This cannot be used to trigger events where the registered type
			specifies "EventData_NotEmpty".
		---------------------------------------------------------------------*/
		void trigger(const string &eventType);

		/*---------------------------------------------------------------------
			Run through the queue and notify listeners, then purge the queue
		---------------------------------------------------------------------*/
		void notifyQueued(uint32_t maxMillis);

		/*---------------------------------------------------------------------
			Registers an event type so that it may be triggered or raised. The
			RegisteredEvent implementation will determine if the event can be
			triggered from script or just code. Returns true if registration
			is successful.
		---------------------------------------------------------------------*/
		bool registerEventType(const string &eventType, const RegEventPtr &regPtr);

		/*---------------------------------------------------------------------
			Returns a shared ptr to RegisteredEvent metadata for an event type
		---------------------------------------------------------------------*/
		RegEventPtr getRegEventPtr(const string &eventType) const
		{
			RegEventMap::const_iterator ri = m_regEventMap.find(eventType);
			if (ri != m_regEventMap.end()) return RegEventPtr(ri->second);
			return RegEventPtr(); // return a null internal pointer if not found
		}

		/*---------------------------------------------------------------------
			Returns true if event type has already been registered in the
			RegisteredEvent table (doesn't care about listener table)
		---------------------------------------------------------------------*/
		bool isEventTypeRegistered(const string &eventType) const { return (m_regEventMap.find(eventType) != m_regEventMap.end()); }
		
		/*---------------------------------------------------------------------
			Returns true if added, false if already exists, with priority
			(1 is highest priority, 0 is no priority or FIFO order).
			If event type does not exist it is added. An O(n) operation since
			it traverses the existing list for duplicates.
		---------------------------------------------------------------------*/
		bool registerListener(const string &eventType, EventListener *lPtr, uint32_t priority = 0);
		
		/*---------------------------------------------------------------------
			Removes a listener from an event type, if event type has no
			more listeners it is removed
		---------------------------------------------------------------------*/
		bool removeListener(const string &eventType, EventListener *lPtr);

		/*---------------------------------------------------------------------
			Multithread safe raise methods
		---------------------------------------------------------------------*/
		void raiseThreadSafe(const EventPtr &ePtr);
		void raiseThreadSafe(const string &eventType);

		// Constructor / Destructor
		static shared_ptr<EventManager> create(unique_ptr<EventSnooper> &es);
		~EventManager();
};

typedef shared_ptr<EventManager>	EventManagerPtr;
typedef weak_ptr<EventManager>		EventManagerWeakPtr;

/*=============================================================================
class EventSnooper
	This listener uses the wildcard to receive all events. The return value of
	handleAllEvents is irrelevant in this case, and listener priority is unique
	to the wildcard listener list. Wildcard listeners are always processed
	before the specific listeners for each event.
=============================================================================*/
class EventSnooper : public EventListener {
	private:
		//bool handleSpecificEvent(const EventPtr &ePtr);
		bool handleAllEvents(const EventPtr &ePtr);

	public:
		explicit EventSnooper();
		virtual ~EventSnooper();
};