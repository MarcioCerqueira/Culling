#ifndef AABB_H
#define AABB_H

struct AABB
{
	glm::vec3 minX;
	glm::vec3 maxX;
	glm::vec3 minY;
	glm::vec3 maxY;
	glm::vec3 minZ;
	glm::vec3 maxZ;
	//Mesh
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

#endif