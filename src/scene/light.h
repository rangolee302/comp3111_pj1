#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color, double aa = 0.0, double bb = 0.0, double cc = 1.0)
		: Light( scene, color ), position( pos ), a(aa), b(bb), c(cc) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	virtual vec3f getPosition(const vec3f& P) const {
		return position;
	}
	double a;
	double b;
	double c;

protected:
	vec3f position;
};

class SpotLight
	: public Light
{
public:
	SpotLight(Scene *scene, const vec3f& pos, double coneAn, vec3f& coneD, const vec3f& color, double aa = 0.0, double bb = 0.0, double cc = 1.0)
		: Light(scene, color), position(pos), coneAngle(coneAn), coneDir(coneD), a(aa), b(bb), c(cc) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
	virtual vec3f getPosition(const vec3f& P) const {
		return position;
	}
	double a;
	double b;
	double c;
	float coneAngle;
	vec3f coneDir;
protected:
	vec3f position;

};

#endif // __LIGHT_H__
