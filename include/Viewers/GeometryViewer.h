#ifndef MYGLGEOMETRYVIEWER_H
#define MYGLGEOMETRYVIEWER_H

#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Scene/Mesh.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"

class GeometryViewer
{

public:
	GeometryViewer();
	void configureAmbient(int windowWidth, int windowHeight);
	void configureGBuffer(GLuint vertexMap, GLuint normalMap, GLuint colorMap);
	void configureLight();	
	void configureLinearization();
	void configurePhong(glm::vec3 lightPosition, glm::vec3 cameraPosition);
	void drawPlane(float x, float y, float z);
	void drawMesh(GLuint *VBOs, int numberOfIndices, int numberOfTexCoords, std::vector<Texture> textures, int numberOfColors);
	glm::mat4 getProjectionMatrix() { return projection; }
	glm::mat4 getViewMatrix() { return view; }
	glm::mat4 getModelMatrix() { return model; }
	void loadVBOs(GLuint *VBOs, Mesh *scene, bool drawBoundingBox = false);
	float getFOV() { return fov; }
	float getNear() { return zNear; }
	float getFar() { return zFar; }
	void setEye(glm::vec3 eye) { this->eye = eye; }
	void setLook(glm::vec3 look) { this->look = look; }
	void setShaderProg(GLuint shaderProg) { this->shaderProg = shaderProg; }
	void setUp(glm::vec3 up) { this->up = up; }
	void setProjectionMatrix(glm::mat4 projection) { this->projection = projection; }
	void setViewMatrix(glm::mat4 view) { this->view = view; }
	void setModelMatrix(glm::mat4 model) { this->model = model; }
	
private:
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
	glm::mat3 normalMatrix;
	glm::vec3 eye;
	glm::vec3 look;
	glm::vec3 up;
	float fov;
	float zNear;
	float zFar;
	GLuint shaderProg;
	bool hasNormalMatrixBeenSet;
};

#endif