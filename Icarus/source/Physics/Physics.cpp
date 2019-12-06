/*----==== PHYSICS.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	03/02/2010
	Rev.Date:	03/02/2010
-----------------------------*/

#include "Physics.h"
#include "PhysicsEvents.h"
#include "RigidBody.h"
//#include "../Event/EventManager.h"
//#include "../Event/RegisteredEvents.h"

////////// class PhysicsProcess //////////

/*void PhysicsProcess::onUpdate(float deltaMillis)
{
	// fixed timestep physics update
	// uses time accumulator for stability
	// linear interpolation on the last step prevents temporal aliasing
	mAccumulator += deltaMillis * 0.001f; // Physics uses time unit seconds
	while (mAccumulator >= mFixedDT) {
		// call integrate function on all active objects in scene
		mScene.update(mFixedTime, mFixedDT);

		mFixedTime += mFixedDT; // this will cause precision and overflow problems, need to rethink use of float for this
		mAccumulator -= mFixedDT;
	}
	// get interpolation value between previous and next state
	mAlpha = mAccumulator * mInverseFixedDT;
	// interpolate and send ActorMoved events using for all active objects in scene
	sendInterpolatedStates(mAlpha);
}

void PhysicsProcess::onInitialize()
{
	debugPrintf("%s: process initialized\n", name().c_str());
}

void PhysicsProcess::onFinish()
{
	debugPrintf("%s: process finished\n", name().c_str());
}

void PhysicsProcess::onTogglePause()
{
	if (isPaused()) {
		debugPrintf("%s: process paused\n", name().c_str());
	} else {
		debugPrintf("%s: process unpaused\n", name().c_str());
	}
}*/

/*---------------------------------------------------------------------
	For all active objects in scene, interpolates previous and current
	state, and raises ActorMoved event
---------------------------------------------------------------------*/
/*void PhysicsProcess::sendInterpolatedStates(const float alpha) const
{
	if (!mScene.mActorList.empty()) {
		PhysicsScene::PhysicsActorList::const_iterator li = mScene.mActorList.begin();
		PhysicsScene::PhysicsActorList::const_iterator end = mScene.mActorList.end();
		while (li != end) {
			// get current interpolated position of the actor
			PhysicsActor &actor = *(*li);
			actor.calcInterpolatedState(alpha);
			const State *outState = actor.getInterpolatedState();
			// create and raise the ActorMoved event
			if (outState) {
				EventPtr ePtr(new ActorMovedEvent(	actor.actorID(),
													outState->position(),
													outState->orientation(),
													ActorMovedEvent::System_Physics));
				events.raise(ePtr);
			}
			++li;
		}
	}
}

PhysicsProcess::PhysicsProcess(const string &name, const float fixedDT,
							   const float startTime, PhysicsScene &scene) :
	CProcess(name, CProcess_Run_AlwaysRun, CProcess_Queue_Single),
	mFixedTime(startTime), mAccumulator(0), mFixedDT(fixedDT),
	mInverseFixedDT(fixedDT != 0 ? 1.0f/fixedDT : 0), mAlpha(0),
	mScene(scene)
{
	_ASSERTE(fixedDT > 0);
	if (fixedDT < 0.01f) {
		debugPrintf("%s: Warning! Low fixed timestep, < 0.01 s\n", name.c_str());
	} else if (fixedDT > 0.1f) {
		debugPrintf("%s: Warning! High fixed timestep, > 0.1 s\n", name.c_str());
	}
	// register event type(s)
	events.registerEventType(ActorMovedEvent::sEventType,
							 RegEventPtr(new ScriptCallableCodeEvent<ActorMovedEvent>(EventDataType_NotEmpty)));
}
*/
////////// class PhysicsScene //////////

/*---------------------------------------------------------------------
	Integrates all active objects in scene
---------------------------------------------------------------------*/
/*void PhysicsScene::update(float t, float dt)
{
	if (!mActorList.empty()) {
		PhysicsActorList::const_iterator li = mActorList.begin();
		PhysicsActorList::const_iterator end = mActorList.end();
		while (li != end) {
			(*li)->update(t, dt);
			++li;
		}
	}
}
*/
/*---------------------------------------------------------------------
	Finds interpolated position for all active objects
---------------------------------------------------------------------*/
/*void PhysicsScene::calcInterpolatedStates(const float alpha) const
{
	if (!mActorList.empty()) {
		PhysicsActorList::const_iterator li = mActorList.begin();
		PhysicsActorList::const_iterator end = mActorList.end();
		while (li != end) {
			(*li)->calcInterpolatedState(alpha);
			++li;
		}
	}
}

PhysicsScene::PhysicsScene()
{
	// create the thread process that will process async loading requests
	CProcessPtr procPtr(new PhysicsProcess("PhysicsProcess", 0.1f, 0, *this));
	mIntegratorProcPtr = procPtr;
	procMgr.attach(mIntegratorProcPtr);
}
*/
////////// class PhysicsSceneListener //////////

/*---------------------------------------------------------------------
	Some display setting was changed, requiring a reset of the device
---------------------------------------------------------------------*/
/*bool PhysicsScene::PhysicsSceneListener::handleActorMoved(const EventPtr &ePtr)
{
	ActorMovedEvent &e = *(static_cast<ActorMovedEvent*>(ePtr.get()));
	if (e.systemGen != ActorMovedEvent::System_Physics) {
		debugPrintf("%s: Handled the Actor moved event!\n", name().c_str());
	} else {
		debugPrintf("%s: Ignored Actor moved event, created by Physics system\n", name().c_str());
	}
	return false; // allow event to propagate
}

PhysicsScene::PhysicsSceneListener::PhysicsSceneListener(PhysicsScene &scene) :
	EventListener("PhysicsSceneListener"),
	mScene(scene)
{
	// register event handlers
	IEventHandlerPtr p(new EventHandler<PhysicsSceneListener>(this, &PhysicsSceneListener::handleActorMoved));
	registerEventHandler(ActorMovedEvent::sEventType, p, 0); // this is a FIFO listener
}
*/
////////// class PhysicsActor //////////

/*---------------------------------------------------------------------
	Returns the interpolated state of the actor
---------------------------------------------------------------------*/
const State * PhysicsActor::getInterpolatedState() const
{
	if (mRigidBody) {
		return &mRigidBody->interpolatedState();
	}
	return 0;
}

/*---------------------------------------------------------------------
	If active, integrates the state by one time step of dt seconds
---------------------------------------------------------------------*/
void PhysicsActor::update(float t, float dt)
{
	if (mActive) {
		mRigidBody->update(t, dt);
	}
}

/*---------------------------------------------------------------------
	If active, finds interpolated position
---------------------------------------------------------------------*/
void PhysicsActor::calcInterpolatedState(const float alpha)
{
	if (mRigidBody) {
		mRigidBody->calcInterpolatedState(alpha);
	}
}
