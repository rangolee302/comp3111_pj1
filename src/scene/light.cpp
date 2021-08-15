#include <cmath>

#include "light.h"
#define 	M_PI   3.14159265358979323846

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f lightDirFromP = getDirection(P).normalize();
	isect i;
	ray r(P, lightDirFromP);
	vec3f result = getColor(P);

	if (scene->intersect(r, i)) {
		vec3f intersected = r.at(i.t);
		const Material& m = i.getMaterial();
		if (!m.kt.iszero()) {
			result[0] = result[0] * m.kt[0];
			result[1] = result[1] * m.kt[1];
			result[2] = result[2] * m.kt[2];
			return result;
		}
		return vec3f(0, 0, 0);
	}

    return result;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double dist = (position - P).length();

	double atten = 1.0 / (a + b * dist + c * dist * dist);
	if (atten > 1.0) atten = 1.0;

	return atten;
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f lightDirFromP = getDirection(P).normalize();
	isect i;
	ray r(P, lightDirFromP);
	vec3f result = getColor(P);

	if (scene->intersect(r, i)) {
		vec3f intersected = r.at(i.t);
		const Material& m = i.getMaterial();
		if (!m.kt.iszero()) {
			result[0] = result[0] * m.kt[0];
			result[1] = result[1] * m.kt[1];
			result[2] = result[2] * m.kt[2];
			return result;
		}
		return vec3f(0, 0, 0);
	}

	return result;
}

double SpotLight::distanceAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double dist = (position - P).length();

	double atten = 1.0 / (a + b * dist + c * dist * dist);
	if (atten > 1.0) atten = 1.0;

	return atten;
}

vec3f SpotLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f SpotLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}


vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	vec3f lightDirFromP = getDirection(P).normalize();
	isect i;
	ray r(P, lightDirFromP);
	vec3f result = getColor(P);

	vec3f coneDirHere = coneDir.normalize();
	vec3f ray = -lightDirFromP;
	double lightToNormalAngle = acos(ray.normalize().dot(coneDirHere));
		
	if (lightToNormalAngle > coneAngle) {
		return vec3f(0, 0, 0);
	} else {
		if (scene->intersect(r, i)) {
			return vec3f(0, 0, 0);
		}
	}

	return result;
}