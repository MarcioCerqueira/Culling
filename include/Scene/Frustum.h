#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <math.h>
#include <glm\glm.hpp>
#include "Scene\Mesh.h"
#include "Scene\Plane.h"

class Frustum
{

public:
	Frustum();
	void updateMesh(glm::mat4 projectionMatrix);
	bool cull(AABB boundingBox, bool planeCulling = true);
	bool cull(Sphere sphere, bool planeCulling = true);
	Mesh getMesh() { return frustumMesh; }
	
private:
	Mesh frustumMesh;
	Plane planes[6];
};
#endif