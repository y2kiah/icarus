/*----==== PHYSICSEVENTS.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	03/02/2010
	Rev.Date:	03/02/2010
-----------------------------------*/

#include "PhysicsEvents.h"

////////// class ActorMovedEvent //////////

//const string ActorMovedEvent::sEventType("SYS_ACTOR_MOVED");

/*---------------------------------------------------------------------
	This is called to construct script data out of a code-defined event
---------------------------------------------------------------------*/
/*void ActorMovedEvent::buildScriptData()
{
	mScriptData.clear();
	mScriptData.push_back(AnyVarsValue("actorID", actorID));
	mScriptData.push_back(AnyVarsValue("newPositionX", newPosition.x));
	mScriptData.push_back(AnyVarsValue("newPositionY", newPosition.y));
	mScriptData.push_back(AnyVarsValue("newPositionZ", newPosition.z));
	mScriptData.push_back(AnyVarsValue("newRotationW", newRotation.w));
	mScriptData.push_back(AnyVarsValue("newRotationX", newRotation.x));
	mScriptData.push_back(AnyVarsValue("newRotationY", newRotation.y));
	mScriptData.push_back(AnyVarsValue("newRotationZ", newRotation.z));
	mScriptData.push_back(AnyVarsValue("systemGen", systemGen));
	mScriptDataBuilt = true;
}*/

/*---------------------------------------------------------------------
	The script-called constructor first sets all data equal to their
	current values in Engine::mSettings, so if they aren't all supplied
	by script, the setting will go unchanged.
---------------------------------------------------------------------*/
/*ActorMovedEvent::ActorMovedEvent(const AnyVars &eventData) :
	ScriptableEvent(eventData),
	actorID(0), systemGen(System_Scripting)
{
	for (AnyVars::const_iterator i = eventData.begin(); i != eventData.end(); ++i) {
		try {
			if (_stricmp(i->first.c_str(), "actorID") == 0) {
				actorID = any_cast<int>(i->second);
			} else if (_stricmp(i->first.c_str(), "newPositionX") == 0) {
				newPosition.x = any_cast<float>(i->second);
			} else if (_stricmp(i->first.c_str(), "newPositionY") == 0) {
				newPosition.y = any_cast<float>(i->second);
			} else if (_stricmp(i->first.c_str(), "newPositionZ") == 0) {
				newPosition.z = any_cast<float>(i->second);
			} else if (_stricmp(i->first.c_str(), "newRotationW") == 0) {
				newRotation.w = any_cast<float>(i->second);
			} else if (_stricmp(i->first.c_str(), "newRotationX") == 0) {
				newRotation.x = any_cast<float>(i->second);
			} else if (_stricmp(i->first.c_str(), "newRotationY") == 0) {
				newRotation.y = any_cast<float>(i->second);
			} else if (_stricmp(i->first.c_str(), "newRotationZ") == 0) {
				newRotation.z = any_cast<float>(i->second);
			}
			// ignore systemGen property since we already know it's coming from Script
		} catch (const boost::bad_any_cast &ex) {
			// nothing happens with a bad datatype in release build, silently ignores
			debugPrintf("DeviceChangeEvent: bad_any_cast \"%s\"\n", ex.what());
		}
	}
}*/