/*----==== AIRFRAME.H ====----
	Author:	Jeffrey Kiah
	Date:	2/3/2009
----------------------------*/

#pragma once

#include "../Math Code/Plane3.h"

/*=============================================================================
=============================================================================*/
class LiftingSurface {
	private:
		Plane3		sfcPlane;		// wing chord plane contains unit normal vector
		Vector3f	aeroCenter;		// aerodynamic center in body coordinates, ft.
		Airfoil		airfoil;		// 
		float		span;			// span of the surface (b), ft.
		float		rootChord;		// chord at the root (Croot), ft.
		float		tipChord;		// chord at the tip (Ctip), ft.
		float		planformArea;	// area of the trapezoid (S), ft.^2
		float		aspectRatio;	// aspect ratio (AR)
		float		taperRatio;		// taper ratio (lambda)
		float		MAC;			// mean aerodynamic chord length (cbar), ft.
		float		spanEfficiency;	// a.k.a. Oswald Efficiency factor (e)
		float		rootIncidence;	// angle of incidence at the wing root, radians
		float		dihedral;		// angle of dihedral, radians
		float		leSweep;		// leading edge sweep angle, radians
		float		CMac;			// coefficient of moment at aerodynamic center MAYBE DON'T NEED SINCE DEFINED FOR AIRFOIL??

	public:
		explicit LiftingSurface() {}
		~LiftingSurface() {}
};