#ifndef SCENELOADER_H
#define SCENELOADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include "Scene\Mesh.h"
#include "Scene\Image.h"

class SceneLoader
{

public:
	SceneLoader(char *filename);
	void load();
	std::vector<Mesh> getScene() { return meshes; }
	float* getCameraPosition() { return cameraPosition; }
	float* getCameraAt() { return cameraAt; }
	float* getLightPosition() { return lightPosition; }
	float* getLightAt() { return lightAt; }
	float* getDebugPosition() { return debugPosition; }
	float* getDebugAt() { return debugAt; }
private:
	void load(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	unsigned int loadTextureFromFile(const char *path, const std::string &directory);

	std::vector<Mesh> meshes;
	std::vector<Texture> texturesLoaded;
	std::fstream file;
	float cameraPosition[3];
	float cameraAt[3];
	float lightPosition[3];
	float lightAt[3];
	float debugPosition[3];
	float debugAt[3];
	std::string directory;
};

#endif