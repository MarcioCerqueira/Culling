#include "Scene\Plane.h"

Plane::Plane() 
{
}

void Plane::setPoints(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3)
{

	glm::vec3 aux1, aux2;
	aux1 = point1 - point2;
	aux2 = point3 - point2;
	normal = glm::normalize(glm::cross(aux2, aux1));
	d = -glm::dot(normal, point1);

}

float Plane::distance(glm::vec3 point)
{
	return d + glm::dot(normal, point);
}