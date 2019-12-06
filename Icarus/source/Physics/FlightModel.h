/*----==== FLIGHTMODEL.H ====----
	Author:	Jeffrey Kiah
	Date:	2/2/2009
-------------------------------*/

#pragma once

#include "Math/IMath.h"

/*=============================================================================
class FlightModel
=============================================================================*/
class FlightModel {
	private:
	public://TEMP
		///// Flight Force Coefficients /////

		// --------------------------------------------------------------------
		// Returns:		Slope per radian (dCL/dAlpha = CLalpha) of the linear portion
		//				of the Coefficient of Lift (CL) curve for a 3d lifting surface
		// Parameters:	span (b), ft.
		//				rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		//				leadingEdgeSweep (Lambda_le), radians
		//				mach (M)
		//				zeroMachSlope (a0) - use 2*pi if unknown
		// Description:	CLalpha = (2*pi*AR)/[2 + sqrt([AR^2*B^2/k^2]*[1+(tan^2(Lambda_c2)/B^2)]+4)]
		//				where:	AR = aspect Ratio
		//						B = compressibility correction
		//						k = a0/(2*pi)
		//						Lambda_c2 = half-chord sweep angle
		// --------------------------------------------------------------------
		static float calcCLalpha(float span, float rootChord, float tipChord, float leadingEdgeSweep,
								 float mach, float zeroMachSlope)
		{
			float S = calcPlanformArea(rootChord, tipChord, span);
			float AR = calcAspectRatio(span, S);
			float B = calcCompressibilityCorrection(mach);
			if (B == 0.0f) { B = 0.0000001f; }
			float k = zeroMachSlope / (TWO_PIf);
			if (k == 0.0f) { k = 1.0f; }
			float tanLambda_c2 = calcTanHalfChordSweep(leadingEdgeSweep, rootChord, tipChord, span);
			float tSlope = (mach < 1.0f) ? TWO_PIf : 4.0f; // subsonic theoretical slope is 2pi, supersonic is 4
			return (tSlope * AR) /
					(2.0f + sqrtf((AR*AR*B*B)/(k*k) * (1.0f + ((tanLambda_c2*tanLambda_c2)/(B*B))) + 4.0f));
		}

		// --------------------------------------------------------------------
		// Returns:		Compressibility Correction (B)
		// Parameters:	mach (M)
		// Description:	B = sqrt(M^2 - 1) or B = sqrt(1 - M^2)
		// --------------------------------------------------------------------
		static float calcCompressibilityCorrection(float mach)
		{
			return ((mach >= 1.0f) ? (sqrtf(mach*mach-1.0f)) : (sqrtf(1.0f-mach*mach)));
		}

		// --------------------------------------------------------------------
		// Returns:		Coefficient of Drag Induced by Lift (CDi)
		// Parameters:	Coefficient of Lift (CL)
		//				aspectRatio (AR)
		//				spanEfficiency (e) = 1.0 for eliptically loaded wing
		// Description:	CDi = CL^2 / (pi * AR * e)
		// --------------------------------------------------------------------
		static float calcCDInduced(float CL, float aspectRatio, float spanEfficiency)
		{
			return (CL*CL) / (PIf * aspectRatio * spanEfficiency);
		}

		///// Pitch Stability Derivatives /////



		///// Directional Stability Derivatives /////

		// --------------------------------------------------------------------
		// Returns:		Slope per radian (dCN/dBeta = CNbeta) of the dihedral
		//				effect of the wing on directional stability
		// Parameters:	gamma (G), radians
		//				coefficient of lift (CL)
		// Description:	(CNbeta)G,wing = -0.75*G*CL
		// --------------------------------------------------------------------
		static float calcCNbetaWingDihedral(float gamma, float CL)
		{
			return -0.75f * gamma * CL;
		}

		// --------------------------------------------------------------------
		// Returns:		Slope per radian (dCN/dBeta = CNbeta) of the sweep
		//				effect of the wing on directional stability
		// Parameters:	coefficient of lift (CL)
		//				span (b), ft.
		//				rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		//				leadingEdgeSweep (Lambda_le), radians
		// Description:	(CNbeta)L,wing = (CL^2)[(1/(4*pi*AR)) - (tanLambda_c4/(pi*AR*(AR+(4*cosLambda_c4))))
		//					* (cosLambda_c4 - AR/2 - AR^2/(8*cosLambda_c4) - (6*Xa_bar*sinLambda_c4/AR))]
		//				where:	AR = aspect Ratio
		//						Lambda_c4 = quarter-chord sweep angle
		//						Xa_bar = (Xcg - Xac) / MAC
		// --------------------------------------------------------------------
		static float calcCNbetaWingSweep(float CL, float span, float rootChord, float tipChord, float leadingEdgeSweep)
		{
			float S = calcPlanformArea(rootChord, tipChord, span);
			float AR = calcAspectRatio(span, S);
			float tanLambda_c4 = calcTanQuarterChordSweep(leadingEdgeSweep, rootChord, tipChord, span);
			float Lambda_c4 = atanf(tanLambda_c4);
			float cosLambda_c4 = cosf(Lambda_c4);
			float Xa_bar = 0.0f; // TEMP

			return (CL*CL) * ( (1.0f/(4.0f*PIf*AR)) - (tanLambda_c4/(PIf*AR*(AR+4.0f*cosLambda_c4))) *
				(cosLambda_c4 - (0.5f*AR) - ((AR*AR)/(8.0f*cosLambda_c4)) - (6.0f * Xa_bar * sinf(Lambda_c4) / AR)) );
		}

		// --------------------------------------------------------------------
		// Returns:		Slope per radian (dCN/dBeta = CNbeta) of the fuselage
		//				with wing contribution to directional stability
		// Parameters:	Wing-body interference factor (KN)
		//				Reynolds number factor (KRl)
		//				bodySideArea (Sbs), ft.^2
		//				planformArea (S), ft.^2
		//				lengthFuselage (lf), ft.
		//				span (b), ft.
		// Description:	(CNbeta)B(w) = (-KN)(KRl)(Sbs/S)(lf/b)
		// --------------------------------------------------------------------
		static float calcCNbetaBody(float KN, float KRl, float bodySideArea, float planformArea,
									float lengthFuselage, float span)
		{
			if (planformArea == 0.0f) { planformArea = 0.0000001f; }
			if (span == 0.0f) { span = 0.0000001f; }
			return -KN * KRl * (bodySideArea / planformArea) * (lengthFuselage / span);
		}

		// --------------------------------------------------------------------
		// Returns:		Slope per radian (dCN/dBeta = CNbeta) of the vertical
		//				stabilizer contribution to directional stability
		// Parameters:	
		// Description:	
		// --------------------------------------------------------------------
		static float calcCNbetaTail(float k)
		{
			return 0.0f;
		}

		///// Geometry Functions /////

		// --------------------------------------------------------------------
		// Returns:		Planform Area (S), ft.^2
		// Parameters:	rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		//				span (b), ft.
		// Description:	S = b[(Croot + Ctip)/2]
		// --------------------------------------------------------------------
		static float calcPlanformArea(float rootChord, float tipChord, float span)
		{
			return span * ((rootChord + tipChord) * 0.5f);
		}

		// --------------------------------------------------------------------
		// Returns:		Mean Aerodynamic Chord Length (MAC or C), ft.
		// Parameters:	rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		// Description:	MAC = (2/3)[(Croot+Ctip) - ([Croot*Ctip]/[Croot+Ctip])]
		// --------------------------------------------------------------------
		static float calcMACLength(float rootChord, float tipChord)
		{
			float c = rootChord + tipChord;
			return (2.0f / 3.0f) * (c - ((rootChord * tipChord) / c));
		}

		// --------------------------------------------------------------------
		// Returns:		Aspect Ratio (A)
		// Parameters:	span (b), ft.
		//				planformArea (S), ft.^2
		// Description:	A = b^2 / S
		// --------------------------------------------------------------------
		static float calcAspectRatio(float span, float planformArea)
		{
			if (planformArea == 0.0f) return 0.0f;
			return (span * span) / planformArea;
		}

		// --------------------------------------------------------------------
		// Returns:		Tangent of Half-Chord Sweep (Lambda_c/2)
		// Parameters:	leadingEdgeSweep (Lambda_le), radians
		//				rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		//				span (b), ft.
		// Description:	tan(Lambda_c/2) = tan(Lambda_le) - ((Croot - Ctip) / b)
		// --------------------------------------------------------------------
		static float calcTanHalfChordSweep(float leadingEdgeSweep, float rootChord, float tipChord, float span)
		{
			if (span == 0.0f) return tanf(leadingEdgeSweep);
			return tanf(leadingEdgeSweep) - ((rootChord - tipChord) / span);
		}

		// --------------------------------------------------------------------
		// Returns:		Tangent of Quarter-Chord Sweep (Lambda_c/4)
		// Parameters:	leadingEdgeSweep (Lambda_le), radians
		//				rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		//				span (b), ft.
		// Description:	tan(Lambda_c/4) = tan(Lambda_le) - ((Croot - Ctip) / 2b)
		// --------------------------------------------------------------------
		static float calcTanQuarterChordSweep(float leadingEdgeSweep, float rootChord, float tipChord, float span)
		{
			if (span == 0.0f) return tanf(leadingEdgeSweep);
			return tanf(leadingEdgeSweep) - ((rootChord - tipChord) / (2.0f * span));
		}

		// --------------------------------------------------------------------
		// Returns:		Leading Edge Sweep (Lambda_le), radians
		// Parameters:	quarterChordSweep (Lambda_c/4), radians
		//				rootChord (Croot), ft.
		//				tipChord (Ctip), ft.
		//				span (b), ft.
		// Description:	Lambda_le = arctan(tan(Lambda_c/4) + ((Croot - Ctip) / 2b))
		// --------------------------------------------------------------------
		static float calcLeadingEdgeSweep(float quarterChordSweep, float rootChord, float tipChord, float span)
		{
			if (span == 0.0f) return 0.0f;
			return atanf(tanf(quarterChordSweep) + ((rootChord - tipChord) / (2.0f * span)));
		}

	public:

		explicit FlightModel() {}
		~FlightModel() {}
};