/*----==== RIGIDBODY.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	03/03/2010
	Rev.Date:	03/03/2010
-------------------------------*/

#include "RigidBody.h"

////////// class RigidBody //////////

/*---------------------------------------------------------------------
	Integrates the rigid body state by one time step of dt seconds
---------------------------------------------------------------------*/
void RigidBody::update(float t, float dt)
{
	mPreviousState = mCurrentState;
	switch (mIntegrator) {
		case IntegratorType_Euler:
			EulerIntegrator::integrate(mCurrentState, t, dt);
			break;
		case IntegratorType_RK4:
			RK4Integrator::integrate(mCurrentState, t, dt);
			break;
	}
}
