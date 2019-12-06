/*----==== PHYSICSEVENTS.H ====----
	Author:		Jeff Kiah
	Orig.Date:	03/02/2010
	Rev.Date:	03/02/2010
---------------------------------*/

#pragma once

#include <cstdint>
//#include "../Event/Event.h"
#include "Math/Vector3f.h"
#include "Math/Quaternion.h"

///// STRUCTURES /////

/*=============================================================================
class ActorMovedEvent
	Triggered from script or code when an actor	has moved. Event data includes
	Actor ID, new position Vector and rotation Quaternion. SystemGen is the
	subsystem responsible for moving the object that raised this event (e.g.
	Physics, Input, Network, Scripting, etc.). In general, any subsystem that
	is capable of moving an actor should also listen for this event and respond
	to other subsystems moving the same actor by transforming it internally.
	The listener can check the systemGen parameter in the event to ignore
	events raised by itself.
=============================================================================*/
/*class ActorMovedEvent : public ScriptableEvent {
	public:
		///// DEFINITIONS /////
		enum SystemGen : uint8_t {
			System_Scripting = 0,
			System_Physics
		};

		///// VARIABLES /////
		// Static
		static const string sEventType;

		// Member
		int			actorID;
		Vector3f	newPosition;
		Quaternion	newRotation;
		SystemGen	systemGen;

		///// FUNCTIONS /////
		virtual const string & type() const { return sEventType; }
		virtual void serialize(ostream &out) const {}
		virtual void deserialize(istream &in) {}
*/
		/*---------------------------------------------------------------------
			This is called to construct script data out of a code-defined event
		---------------------------------------------------------------------*/
/*		virtual void buildScriptData();

		// Constructors
		explicit ActorMovedEvent(int _actorID, const Vector3f &_pos,
								 const Quaternion &_rot, SystemGen _systemGen) :
			ScriptableEvent(),
			actorID(_actorID), newPosition(_pos), newRotation(_rot), systemGen(_systemGen)
		{}
*/
		/*---------------------------------------------------------------------
			The script-called constructor first sets all data equal to their
			current values in Engine::mSettings, so if they aren't all supplied
			by script, the setting will go unchanged.
		---------------------------------------------------------------------*/
/*		explicit ActorMovedEvent(const AnyVars &eventData);

		// Destructor
		virtual ~ActorMovedEvent() {}
};
*/