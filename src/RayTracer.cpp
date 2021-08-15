// The main ray tracer.

#include <FL/Fl_JPEG_Image.H>
#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"
#include <sstream>
extern TraceUI* traceUI;

#define 	M_PI   3.14159265358979323846

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
	vec3f result = vec3f(0.0, 0.0, 0.0);
	if (traceUI->getSuperSampleLevel() == 0 || traceUI->getSuperSampleLevel() == 1) {
		ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
		scene->getCamera()->rayThrough(x, y, r);
		result = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), traceUI->getDepth(),x,y).clamp();
	} else {
		for (int i = 0; i < traceUI->getSuperSampleLevel(); i++) {
			for (int j = 0; j < traceUI->getSuperSampleLevel(); j++) {
				ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
				scene->getCamera()->rayThrough(x, y, r);
				scene->getCamera()->rayThrough(x - (0.5 / (traceUI->getSuperSampleLevel() - 1)) / double(buffer_width) + (i / (traceUI->getSuperSampleLevel() - 1)) / double(buffer_width), y - (0.5 / (traceUI->getSuperSampleLevel() - 1)) / double(buffer_height) + (j / (traceUI->getSuperSampleLevel() - 1)) / double(buffer_height), r);
				result = result + traceRay(scene, r, vec3f(1.0, 1.0, 1.0), traceUI->getDepth(),x,y).clamp();
			}
		}
		result = result / (traceUI->getSuperSampleLevel() * traceUI->getSuperSampleLevel());
	}

	return result;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, double x, double y )
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		// direct color
		vec3f color = vec3f(0.0, 0.0, 0.0);
		//if (i == NULL)
		//	OutputDebugStringW(L"asdf");
		const Material& m = i.getMaterial();
		vec3f intersectedP = r.at(i.t);
		vec3f directColor = m.shade(scene, r, i);
		color += directColor;

		// depth checking
		if (depth <= 0) return color;
		if(thresh.length() < traceUI->getAdaptive()) return color;
		vec3f newThresh = thresh;

		// reflective indirect color
		if (!m.kr.iszero()) {
			vec3f reflectDir = r.getDirection().normalize() - 2.0 * i.N.normalize().dot(r.getDirection().normalize()) * i.N.normalize();
			ray reflectedR(intersectedP, reflectDir.normalize());
			newThresh = prod(newThresh, m.kr);
			vec3f reflectedColor = traceRay(scene, reflectedR, newThresh, (depth - 1),x,y);
			reflectedColor[0] = reflectedColor[0] * m.kr[0];
			reflectedColor[1] = reflectedColor[1] * m.kr[1];
			reflectedColor[2] = reflectedColor[2] * m.kr[2];
			color = color + reflectedColor;
		}

		// refractive indirect color
		if (!m.kt.iszero())
		{
			vec3f normal = i.N.normalize();
			double angle_i = normal.dot(r.getDirection().normalize());
			double n_i = 1.0;
			double n_t = m.index;
			double n = n_i / n_t;

			if (angle_i < 0)
				angle_i = -1.0 * angle_i;
			else {
				normal = -normal;
				n = n_t/n_i;
			}

			double TIR = 1 - n * n * (1 - angle_i * angle_i);
			if (TIR >= 0) {
				vec3f refractedDir = n * r.getDirection().normalize();
				double c1 = angle_i;
				double c2 = sqrt(1.0 - n * n * (1.0 - angle_i * angle_i));
				refractedDir = refractedDir + (n * c1 - c2) * normal;
				ray refractedR(intersectedP, refractedDir);
				newThresh = prod(newThresh, m.kt);
				vec3f refractedColor = traceRay(scene, refractedR, newThresh, (depth - 1),x,y);
				refractedColor[0] = refractedColor[0] * m.kt[0];
				refractedColor[1] = refractedColor[1] * m.kt[1];
				refractedColor[2] = refractedColor[2] * m.kt[2];
				color = color + refractedColor;
			}
		}

		return color;
	}
	else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		if (curBg) {
			return vec3f(0.0, 0.0, 0.0);
		}
		else {
			ostringstream temp;
			temp << curBg->max_size();
			//OutputDebugStringA((temp.str().c_str()));
			return vec3f(0.0,0.0,0.0);
		}
	}
}


RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	curBg = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
	delete curBg;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

bool RayTracer::loadBG(char* bg) {
	if (bg==NULL)
		return false;

	//curBg = new Fl_JPEG_Image(bg);
	
	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}