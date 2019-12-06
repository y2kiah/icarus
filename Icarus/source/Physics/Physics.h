/*----==== PHYSICS.H ====----
	Author:		Jeff Kiah
	Orig.Date:	03/02/2010
	Rev.Date:	03/02/2010
---------------------------*/

#pragma once

#include <list>
#include <memory>
#include <boost/noncopyable.hpp>
//#include "../Process/ProcessManager.h"
//#include "../Event/EventListener.h"

using std::list;
using std::shared_ptr;

///// DEFINITIONS /////

//class PhysicsScene;
//class PhysicsActor;
//typedef shared_ptr<PhysicsActor>	PhysicsActorPtr;

///// STRUCTURES /////

/*=============================================================================
class PhysicsProcess
=============================================================================*/
/*class PhysicsProcess : public CProcess {
	private:
		///// VARIABLES /////
		float	mFixedTime;		// all times in unit seconds
		float	mAccumulator;
		float	mFixedDT;
		float	mInverseFixedDT;
		float	mAlpha;			// interpolation value
		
		PhysicsScene &mScene;	// reference to the PhysicsScene that started this process
*/
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			For all active objects in scene, interpolates previous and current
			state, and raises ActorMoved event
		---------------------------------------------------------------------*/
/*		void sendInterpolatedStates(const float alpha) const;

	public:
		virtual void onUpdate(float deltaMillis);
		virtual void onInitialize();
		virtual void onFinish();
		virtual void onTogglePause();

		explicit PhysicsProcess(const string &name, const float fixedDT,
								const float startTime, PhysicsScene &scene);
		virtual ~PhysicsProcess() {}
};
*/
/*=============================================================================
class PhysicsScene
=============================================================================*/
/*class PhysicsScene : private boost::noncopyable {
	friend class PhysicsProcess;
	friend class PhysicsSceneListener;
	private:
		///// DEFINITIONS /////
		typedef list<PhysicsActorPtr>	PhysicsActorList;
*/
		///// STRUCTURES /////
		/*=====================================================================
		class PhysicsSceneListener
		=====================================================================*/
/*		class PhysicsSceneListener : public EventListener {
			friend class PhysicsScene;
			private:
				PhysicsScene &	mScene;
				bool handleActorMoved(const EventPtr &ePtr);
			public:
				explicit PhysicsSceneListener(PhysicsScene &scene);
		};

		///// VARIABLES /////
		PhysicsActorList	mActorList;
		CProcessPtr			mIntegratorProcPtr;	// pointer to the PhysicsProcess
		
	public:
*/
		///// FUNCTIONS /////
		/*---------------------------------------------------------------------
			Integrates all active objects in scene
		---------------------------------------------------------------------*/
//		void update(float t, float dt);

		/*---------------------------------------------------------------------
			Finds interpolated position for all active objects
		---------------------------------------------------------------------*/
/*		void calcInterpolatedStates(const float alpha) const;

		explicit PhysicsScene();
		~PhysicsScene() {}
};
*/
class RigidBody;
class State;

/*=============================================================================
class PhysicsActor
	TEMP -- change relationship with RigidBody from has-a to is-a
	The RigidBody is-a PhysicsActor just like the Particle or collision-hull
	is-a PhysicsActor
=============================================================================*/
class PhysicsActor {
	private:
		///// VARIABLES /////
		int			mActorID;
		RigidBody *	mRigidBody;	// if null, actor is not dynamic and is just a collision hull
		bool		mActive;	// if active, the actor's rigid body will be integrated

	public:
		///// FUNCTIONS /////
		// Accessors
		int		actorID() const { return mActorID; }
		bool	isActive() const { return mActive; }
		const State * getInterpolatedState() const;

		// Mutators
		/*---------------------------------------------------------------------
			If active, integrates the state by one time step of dt seconds
		---------------------------------------------------------------------*/
		void update(float t, float dt);

		/*---------------------------------------------------------------------
			If active, finds interpolated position
		---------------------------------------------------------------------*/
		void calcInterpolatedState(const float alpha);

		// Constructor / destructor
		explicit PhysicsActor() : mActorID(-1), mRigidBody(0), mActive(false) {}
		explicit PhysicsActor(int actorID) : mActorID(actorID), mRigidBody(0), mActive(false) {}
		~PhysicsActor() {}
};

/*=============================================================================
class PhysicsEngine
=============================================================================*/
class PhysicsEngine {
	public:
/*		// Initialiazation and Maintenance of the Physics World
		virtual bool VInitialize() = 0;
		virtual void VSyncVisibleScene() = 0;
		virtual void VOnUpdate( float deltaSeconds ) = 0; 

		// Initialization of Physics Objects
		virtual void VAddSphere(float radius, IActor *actor, float specificGravity, enum PhysicsMaterial mat) = 0;
		virtual void VAddBox(const Vec3& dimensions, IActor *gameActor, float specificGravity, enum PhysicsMaterial mat) = 0;
		virtual void VAddPointCloud(Vec3 *verts, int numPoints, IActor *gameActor, float specificGravity, enum PhysicsMaterial mat) = 0;
		virtual void VRemoveActor(ActorId id) = 0;

		// Debugging
		virtual void VRenderDiagnostics() = 0;

		// Physics world modifiers
		virtual void VCreateTrigger(const Vec3 &pos, const float dim, int triggerID) = 0;
		virtual void VApplyForce(const Vec3 &dir, float newtons, ActorId aid) = 0;
		virtual void VApplyTorque(const Vec3 &dir, float newtons, ActorId aid) = 0;
		virtual bool VKinematicMove(const Mat4x4 &mat, ActorId aid)=0;

		// Physics actor states
		virtual void VRotateY(ActorId actorId, float angleRadians, float time) = 0;
		virtual float VGetOrientationY(ActorId actorId) = 0;
		virtual void VStopActor(ActorId actorId) = 0;
		virtual void VSetVelocity(ActorId actorId, const Vec3& vel) = 0;
		virtual void VTranslate(ActorId actorId, const Vec3& vec) = 0;

		virtual ~PhysicsEngine() {}*/
};