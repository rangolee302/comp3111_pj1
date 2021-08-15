#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	vec3f intersectedPoint = r.at(i.t);
	const Material& m = i.getMaterial();
	vec3f color = m.ke + prod(m.ka, scene->getIa());

	for (std::list<Light*>::const_iterator it = scene->beginLights(); it != scene->endLights(); ++it) {
		vec3f lightDir = (*it)->getDirection(intersectedPoint).normalize();

		// vec3f colorOfLight = (*it)->getColor(intersectedPoint); // atten will handle this
		vec3f diffuseColor = m.kd;
		diffuseColor[0] = diffuseColor[0] * max(0.0, i.N.normalize().dot(lightDir));
		diffuseColor[1] = diffuseColor[1] * max(0.0, i.N.normalize().dot(lightDir));
		diffuseColor[2] = diffuseColor[2] * max(0.0, i.N.normalize().dot(lightDir));
		vec3f viewDir = (r.getPosition() - intersectedPoint).normalize();
		// vec3f reflectDirR = (2 * (i.N * lightDir) * i.N) - lightDir; // reflection
		vec3f reflectDirR = -1.0 * lightDir.normalize() - 2.0 * i.N.normalize().dot(-1.0 * lightDir.normalize()) * i.N.normalize();

		reflectDirR = reflectDirR.normalize();

		vec3f specColor = m.ks;
		specColor[0] = specColor[0] * pow(max(0.0, reflectDirR.dot(viewDir)), shininess * 128.0);
		specColor[1] = specColor[1] * pow(max(0.0, reflectDirR.dot(viewDir)), shininess * 128.0);
		specColor[2] = specColor[2] * pow(max(0.0, reflectDirR.dot(viewDir)), shininess * 128.0);

		vec3f atten = ((*it)->distanceAttenuation(intersectedPoint)) * ((*it)->shadowAttenuation(intersectedPoint));
		color = color + prod(atten, diffuseColor + specColor);
	}

	return color.clamp();
}
