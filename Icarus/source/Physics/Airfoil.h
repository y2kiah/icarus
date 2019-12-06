/*----==== AIRFOIL.H ====----
	Author:	Jeffrey Kiah
	Date:	2/16/2009
---------------------------*/

#pragma once

#include <string>
#include "../Math/FastMath.h"

/*=============================================================================
class Airfoil
=============================================================================*/
class Airfoil {
	private:
	public://TEMP
		float mCLIntercept;		// coefficient of lift at alpha=0
		float mCLSlope;			// slope of the linear portion of the CL curve (2pi/radian theoretically)
		float mCLLinearRange;	// +/- alpha range from zero of the linear slope (in radians)
		//float 

		std::string		name;

		// --------------------------------------------------------------------
		// Returns:		
		// Parameters:	
		// Description:	
		// --------------------------------------------------------------------
		float flowSeparationCurve(float alpha) {
			//float a1 = 15.0f * DEGTORADf;
			//float deltaAlpha = 0.0f * DEGTORADf;
			//float x0 = 0.5f * (1.0f - tanhf(a1 * (alpha - deltaAlpha)));
			//float c = (1.0f + sqrtf(x0)) * 0.5f;
			//return mCLIntercept + (mCLSlope * c*c * alpha);
			return 0;//mCLIntercept + (mCLMax - mCLIntercept) * tanhf(alpha);
		}
	public:
		explicit Airfoil() {}
		~Airfoil() {}
};