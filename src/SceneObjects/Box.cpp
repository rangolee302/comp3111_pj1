#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
	// Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	vec3f p = r.getPosition();
	vec3f d = r.getDirection().normalize();
	double firstT = -4411.0;
	double secondT = 4411.0;

	for (int k = 0; k < 3; k++) {
		if (abs(d[k]) <= 0.00001 || d[k] == 0.0) {
			if (p[k] < -0.5 || p[k] > 0.5)
				return false;
		}

		double firstTK = (0.5 - p[k]) / d[k];
		double secondTK = (-0.5 - p[k]) / d[k];
		if (secondTK < firstTK) {
			double tmp = secondTK;
			secondTK = firstTK;
			firstTK = tmp;
		}
		if (firstTK > firstT) firstT = firstTK;
		if (secondTK < secondT) secondT = secondTK;
	}

	if (firstT > secondT)
		return false;

	if (firstT < RAY_EPSILON || secondT < RAY_EPSILON)
		return false;

	i.t = firstT;
	i.obj = this;

	vec3f intersectedPoint = r.at(firstT);
	for (int k = 0; k < 3; k++) {
		if (abs(intersectedPoint[k] - (-0.5)) < RAY_EPSILON) {
			i.N = vec3f(-(float)(k == 0), -(float)(k == 1), -(float)(k == 2));
			return true;
		}
		else if (abs(intersectedPoint[k] - (0.5)) < RAY_EPSILON) {
			i.N = vec3f((float)(k == 0), (float)(k == 1), (float)(k == 2));
			return true;
		}
	}

	return false;
}
