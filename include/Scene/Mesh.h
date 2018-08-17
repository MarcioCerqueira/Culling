#ifndef MESH_H
#define MESH_H

#include <malloc.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "Scene/AABB.h"
#include "Scene/Sphere.h"
#include "Scene/Texture.h"

class Mesh
{
public:
	Mesh();
	~Mesh();
	
	void addPoint(glm::vec3 point) { vertices.push_back(point); }
	void addNormal(glm::vec3 normal) { normals.push_back(normal); }
	void addIndex(unsigned int index) { indices.push_back(index); }
	void addTextureCoordinates(glm::vec2 texCoord) { texCoords.push_back(texCoord); }
	void addTexture(std::vector<Texture> map) { textures.insert(textures.end(), map.begin(), map.end()); }
	void addColor(glm::vec3 color) { colors.push_back(color); }
	
	void computeNormals();
	void computeAABB();
	void computeSphere();
	void convertAABBToMesh();
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	
	void translate(float x, float y, float z);
	void scale(float x, float y, float z);
	void rotate(float x, float y, float z);
	
	void* getPointCloud() { return &vertices[0]; }
	void* getNormalVector() { return &normals[0]; }
	void* getIndices() { return &indices[0]; }
	void* getTextureCoordinates() { return &texCoords[0]; }
	void* getColors() { return &colors[0]; }
	std::vector<Texture> getTextures() { return textures; }
	AABB getAABB() { return boundingBox; }
	Sphere getSphere() { return sphere; }
	glm::vec3 getPoint(int index) { return vertices[index]; }

	int getPointCloudSize() { return vertices.size(); }
	int getIndicesSize() { return indices.size(); }
	int getNumberOfTriangles() { return indices.size() / 3; }
	int getTextureCoordsSize() { return texCoords.size(); }
	int getColorSize() { return colors.size(); }

	void setPoint(glm::vec3 point, int index) { vertices[index] = point; }

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
	std::vector<glm::vec2> texCoords;
	std::vector<Texture> textures;
	AABB boundingBox;
	Sphere sphere;
};

#endif