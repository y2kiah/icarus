/*----==== RIGIDBODY.H ====----
	Author:		Jeff Kiah
	Orig.Date:	03/03/2010
	Rev.Date:	03/03/2010
-----------------------------*/

#pragma once

#include "Integrator.h"

/*=============================================================================
class RigidBody
=============================================================================*/
class RigidBody {
	private:
		///// VARIABLES /////
		State			mCurrentState;
		State			mPreviousState;
		State			mInterpolatedState;
		IntegratorType	mIntegrator;

	public:
		///// FUNCTIONS /////
		// Accessors
		const State &	currentState() const { return mCurrentState; }
		const State &	previousState() const { return mPreviousState; }
		const State &	interpolatedState() const { return mInterpolatedState; }
		IntegratorType	integratorType() const { return mIntegrator; }

		// Mutators
		/*---------------------------------------------------------------------
			Integrates the rigid body state by one time step of dt seconds
		---------------------------------------------------------------------*/
		void update(float t, float dt);

		/*---------------------------------------------------------------------
			Stores and returns interpolated values between previous and current
			state, for smooth blending between physics steps. alpha [0,1]
			indicates normalized time between discreet physics steps.
		---------------------------------------------------------------------*/
		const State & calcInterpolatedState(const float alpha)
		{
			mInterpolatedState.interpolate(mPreviousState, mCurrentState, alpha);
			return mInterpolatedState;
		}

		// Constructors
		explicit RigidBody() : mIntegrator(IntegratorType_RK4) {}
		explicit RigidBody(const State &init, IntegratorType type) :
			mCurrentState(init), mPreviousState(init),
			mInterpolatedState(init), mIntegrator(type)
		{}
};