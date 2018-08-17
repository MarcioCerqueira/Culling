#ifndef PLANE_H
#define PLANE_H

#include "glm/glm.hpp"

class Plane
{

public:
	Plane();
	float distance(glm::vec3 point);
	glm::vec3 getNormal() { return normal; }
	void setPoints(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);

private:
	glm::vec3 normal;
	float d;
};
#endif