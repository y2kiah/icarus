/*----==== INTEGRATOR.H ====----
	Author:		Jeff Kiah
	Orig.Date:	02/23/2010
	Rev.Date:	03/03/2010
------------------------------*/

#pragma once

#define INTEGRATOR_USE_SLERP

#include <cstdint>
#include <limits>
#include "Math/Vector3f.h"
#include "Math/Quaternion.h"
#include "Math/IMath.h"

enum IntegratorType : uint8_t {
	IntegratorType_Euler = 0,
	IntegratorType_RK4
};

class RigidComponentTree;

/*=============================================================================
class State
=============================================================================*/
class State {
	friend class Derivative;
	friend class EulerIntegrator;
	friend class RK4Integrator;
	protected:
		///// VARIABLES /////
		// primary values
		Vector3f	mPosition;			// x
		Vector3f	mMomentum;			// p
		Quaternion	mOrientation;		// q
		Vector3f	mAngularMomentum;	// u
		// secondary values
		Vector3f	mVelocity;			// v
		Vector3f	mAngularVelocity;	// w
		Quaternion	mSpin;				// =1/2*w*q
		// constant properties (can change, but assumed constant for each integration step)
		float		mMass;				// m
		float		mInverseMass;		// 1/m
		float		mInertia;			// i
		float		mInverseInertia;	// 1/i

		///// FUNCTIONS /////

		/*---------------------------------------------------------------------
			Any time a constant value changes, some primary values may need to
			be updated in this function to observe laws of conservation of
			linear and angular momentum. Because momentum is conserved in the
			closed system (no external force causing change of momentum),
			secondary values don't need a recalc, the next integration step
			will handle it.
		---------------------------------------------------------------------*/
		void recalculatePrimaries();

	public:
		// Accessors
		const Vector3f & position() const { return mPosition; }
		const Vector3f & momentum() const { return mMomentum; }
		const Quaternion & orientation() const { return mOrientation; }
		const Vector3f & angularMomentum() const { return mAngularMomentum; }
		const Vector3f & velocity() const { return mVelocity; }
		const Vector3f & angularVelocity() const { return mAngularVelocity; }
		const Quaternion & spin() const { return mSpin; }
		float mass() const { return mMass; }
		float inverseMass() const { return mInverseMass; }
		float inertia() const { return mInertia; }
		float inverseInertia() const { return mInverseInertia; }

		// Mutators

		/*---------------------------------------------------------------------
			These functions set constants, so must call recalculatePrimaries.
			Return false if change not accepted.
		---------------------------------------------------------------------*/
		bool setMass(float mass);
		bool setInertiaTensorCubeSize(float size);

		/*---------------------------------------------------------------------
			Any time a primary value changes, a secondary value must be updated
			in this function. This occurs through normal timestep interpolation
			activity, so it is called at least every update frame after
			primaries are set or changed explicitly or implicitly.
		---------------------------------------------------------------------*/
		void recalculateSecondaries();

		/*---------------------------------------------------------------------
			Interpolate from state 0 (delta=0) to state 1 (delta=1)
		---------------------------------------------------------------------*/
		void interpolate(const State &s0, const State &s1, const float delta);

		/*---------------------------------------------------------------------
			Function to find force and torque on rigid body for integration of
			current timestep. Time t provided in case force function depends on
			time. RigidComponentTree will be traversed, all moments summed to
			get resultant force and torque for both linear and rotational
			motion.
		---------------------------------------------------------------------*/
		void resolveMoments(const RigidComponentTree *root, const float t,
							Vector3f &outForce, Vector3f &outTorque) const;

		// Constructors / destructor
		explicit State() :
			mMass(0), mInverseMass(FLT_MAX), mInertia(0), mInverseInertia(FLT_MAX)
		{}
		/*---------------------------------------------------------------------
			Sets inertia tensor assuming a cube with side length *size*
		---------------------------------------------------------------------*/
		explicit State(float mass, float size) :
			mMass(mass),
			mInverseMass(mass != 0 ? 1.0f / mass : FLT_MAX),
			mInertia(1.0f/6.0f*mass*size*size),
			mInverseInertia(mInertia != 0 ? 1.0f / mInertia : FLT_MAX)
		{}
		explicit State(const State &s) :
			mPosition(s.mPosition), mMomentum(s.mMomentum), mOrientation(s.mOrientation),
			mAngularMomentum(s.mAngularMomentum), mVelocity(s.mVelocity),
			mAngularVelocity(s.mAngularVelocity), mSpin(s.mSpin), mMass(s.mMass),
			mInverseMass(s.mInverseMass), mInertia(s.mInertia), mInverseInertia(s.mInverseInertia)
		{}
		~State() {}
};

/*=============================================================================
class Derivative
=============================================================================*/
class Derivative {
	friend class EulerIntegrator;
	friend class RK4Integrator;
	private:
		// linear
		Vector3f	mVelocity;		// dx/dt
		Vector3f	mForce;			// dp/dt
		// angular
		Quaternion	mSpin;			// dq/dt
		Vector3f	mTorque;		// du/dt

	public:
		const Derivative & evaluate(const State &initial, float t)
		{
			mVelocity = initial.velocity();
			mSpin = initial.spin();
			initial.resolveMoments(0, t, mForce, mTorque);
			return *this;
		}

		const Derivative & evaluate(const State &initial, float t, float dt, const Derivative &d)
		{
			State state(initial);
			state.mPosition += d.mVelocity * dt;
			state.mMomentum += d.mForce * dt;
			state.mOrientation += d.mSpin * dt;
			state.mAngularMomentum += d.mTorque * dt;
			state.recalculateSecondaries();
			
			mVelocity = state.mVelocity;
			mSpin = state.mSpin;
			state.resolveMoments(0, t+dt, mForce, mTorque);
			return *this;
		}
};

/*=============================================================================
class EulerIntegrator
=============================================================================*/
class EulerIntegrator {
	public:
		static void integrate(State &state, float t, float dt)
		{
			Derivative d;
			d.evaluate(state, t);

			state.mPosition += d.mVelocity * dt;
			state.mMomentum += d.mForce * dt;
			state.mOrientation += d.mSpin * dt;
			state.mAngularMomentum += d.mTorque * dt;
			
			state.recalculateSecondaries();
		}
};

/*=============================================================================
class RK4Integrator
=============================================================================*/
class RK4Integrator {
	public:
		static void integrate(State &state, float t, float dt)
		{
			Derivative a,b,c,d;
			a.evaluate(state, t);
			b.evaluate(state, t, dt*0.5f, a);
			c.evaluate(state, t, dt*0.5f, b);
			d.evaluate(state, t, dt, c);

			float oneSixthDT = 1.0f/6.0f * dt;
			// dVelocity = (a.mVelocity + 2.0f * (b.mVelocity + c.mVelocity) + d.mVelocity) * oneSixthDT
			Vector3f dVelocity(b.mVelocity); dVelocity.add(c.mVelocity).scale(2.0f).add(a.mVelocity).add(d.mVelocity).scale(oneSixthDT);
			state.mPosition += dVelocity;
			// dForce = (a.mForce + 2.0f * (b.mForce + c.mForce) + d.mForce) * oneSixthDT
			Vector3f dForce(b.mForce); dForce.add(c.mForce).scale(2.0f).add(a.mForce).add(d.mForce).scale(oneSixthDT);
			state.mMomentum += dForce;
			// dOrientation = (a.mSpin + 2.0f * (b.mSpin + c.mSpin) + d.mSpin) * oneSixthDT
			Quaternion dSpin(b.mSpin); dSpin.add(c.mSpin).scale(2.0f).add(a.mSpin).add(d.mSpin).scale(oneSixthDT);
			state.mOrientation += dSpin;
			// dTorque = (a.mTorque + 2.0f * (b.mTorque + c.mTorque) + d.mTorque) * oneSixthDT
			Vector3f dTorque(b.mTorque); dTorque.add(c.mTorque).scale(2.0f).add(a.mTorque).add(d.mTorque).scale(oneSixthDT);
			state.mAngularMomentum += dTorque;

			state.recalculateSecondaries();
		}
};