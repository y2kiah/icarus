/*----==== INTEGRATOR.CPP ====----
	Author:		Jeff Kiah
	Orig.Date:	02/23/2010
	Rev.Date:	03/03/2010
--------------------------------*/

#include "Integrator.h"

////////// class State //////////

/*---------------------------------------------------------------------
	These functions set constants, so must call recalculatePrimaries.
	Return false if change not accepted.
---------------------------------------------------------------------*/
bool State::setMass(float mass)
{
	if (mass >= 0) {
		mMass = mass;
		// inertia is proportional to mass, so change in inertia is proportional to change in mass
		// this is multiply inertia by newMass / oldMass to get new inertia
		mInertia *= mMass * mInverseMass;

		mInverseMass = (mMass != 0 ? 1.0f / mMass : FLT_MAX);
		mInverseInertia = (mInertia != 0 ? 1.0f / mInertia : FLT_MAX);
		// to ensure momentum is conserved after the change in mass, need to recalc momentum
		recalculatePrimaries();
		return true;
	}
	//debugPrintf("State: mass must be set >= 0\n");
	return false;
}

bool State::setInertiaTensorCubeSize(float size)
{
	if (size >= 0) {
		mInertia = 1.0f / 6.0f * mMass * size * size;
		mInverseInertia = (mInertia != 0 ? 1.0f / mInertia : FLT_MAX);
		// to ensure angular momentum is conserved after the change in inertia tensor, need to recalc momentum
		recalculatePrimaries();
		return true;
	}
	//debugPrintf("State: inertia tensor cube size must be set >= 0\n");
	return false;
}

/*---------------------------------------------------------------------
	Any time a constant value changes, some primary values may need to
	be updated in this function to observe laws of conservation of
	linear and angular momentum. Because momentum is conserved in the
	closed system (no external force causing change of momentum),
	secondary values don't need a recalc, the next integration step
	will handle it.
	
	To illustrate, imagine jettisoning a fuel tank from
	an airplane. The mass of the airplane is effectively reduced, which
	reduces the momentum of the airplane, but the fuel tank, now its
	own independant body, inherits the lost momentum from the airplane.
	Total momentum change in the closed system is zero. If we don't
	reduce momentum of the airplane when mass changes, the next
	integration step there would be a large velocity increase (we don't
	want that).
---------------------------------------------------------------------*/
void State::recalculatePrimaries()
{
	// p = v*m
	mMomentum.assign(mVelocity);
	mMomentum.scale(mMass);
	mAngularMomentum.assign(mAngularVelocity);
	mAngularMomentum.scale(mInertia);
}

/*---------------------------------------------------------------------
	Any time a primary value changes, a secondary value must be updated
	in this function. This occurs through normal timestep interpolation
	activity, so it is called at least every update frame after
	primaries are set or changed explicitly or implicitly.
---------------------------------------------------------------------*/
void State::recalculateSecondaries()
{
	// linear
	mVelocity = mMomentum * mInverseMass;
	// angular
	mAngularVelocity.assign(mAngularMomentum);
	mAngularVelocity.scale(mInverseInertia);
	mOrientation.normalize();
	mSpin.assign(mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z, 0);
	mSpin.scale(0.5f).multiply(mOrientation);
}

/*---------------------------------------------------------------------
	Interpolate from state 0 (delta=0) to state 1 (delta=1)
---------------------------------------------------------------------*/
void State::interpolate(const State &s0, const State &s1, const float delta)
{
	mMass = s1.mMass;
	mInverseMass = s1.mInverseMass;
	mInertia = s1.mInertia;
	mInverseInertia = s1.mInverseInertia;

	mPosition.lerp(s0.mPosition, s1.mPosition, delta);
	mMomentum.lerp(s0.mMomentum, s1.mMomentum, delta);
	mAngularMomentum.lerp(s0.mAngularMomentum, s1.mAngularMomentum, delta);

	#ifdef INTEGRATOR_USE_SLERP
	mOrientation.slerp(s0.mOrientation, s1.mOrientation, delta);
	#else
	mOrientation.nlerp(s0.mOrientation, s1.mOrientation, delta);
	#endif

	recalculateSecondaries();
}

/*---------------------------------------------------------------------
	Function to find force and torque on rigid body for integration of
	current timestep. Time t provided in case force function depends on
	time. RigidComponentTree will be traversed, all moments summed to
	get resultant force and torque for both linear and rotational
	motion.
---------------------------------------------------------------------*/
void State::resolveMoments(const RigidComponentTree *root, const float t,
						   Vector3f &outForce, Vector3f &outTorque) const
{
	// sum all moments in the system to get resultant linear force and torque
	/*
	resultantForce = {0,0,0}
	resultantMoment = {0,0,0}
	for each Component cmp {
		Vpoint = Vlinear + Vangular X cmp.Position
		Force = cmp.Force(Vpoint, t, ...)
		resultantForce += Force
		resultantMoment += Force X cmp.Position
	}
	*/
	outForce.assign(0,0,0);
	outTorque.assign(0,0,0);
}

