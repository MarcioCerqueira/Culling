#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <time.h>
#include "Viewers\TextureViewer.h"
#include "Viewers\GeometryViewer.h"
#include "Viewers\shader.h"
#include "Scene\SceneLoader.h"
#include "Scene\Mesh.h"
#include "Scene\Image.h"
#include "Scene\Frustum.h"

enum
{
	GBUFFER_FRAMEBUFFER = 0,
};

enum
{
	GBUFFER_SHADER = 0,
	PHONG_SHADER = 1,
};

enum
{
	GBUFFER_MAP_DEPTH = 0,
	VERTEX_MAP_COLOR = 1,
	NORMAL_MAP_COLOR = 2,
	TEXTURE_MAP_COLOR = 3,
};

//Window size
int windowWidth = 1920;
int windowHeight = 1000;

//Number of frames
int frameCount = 0;
float fps = 0;
int currentTime = 0, previousTime = 0;
char fileName[1000];

TextureViewer textureViewer;
GeometryViewer geometryViewer;

std::vector<Mesh> scene;
SceneLoader *sceneLoader;
Frustum frustum;

GLuint textures[10];
GLuint frameBuffer[2];
std::vector<GLuint*> sceneVBO;

glm::vec3 cameraEye;
glm::vec3 cameraAt;
glm::vec3 cameraUp;

glm::vec3 lightEye;
glm::vec3 lightAt;
glm::vec3 lightUp;

glm::vec3 debugEye;
glm::vec3 debugAt;
glm::vec3 debugUp;

GLuint ProgramObject = 0;
GLuint VertexShaderObject = 0;
GLuint FragmentShaderObject = 0;
GLuint shaderVS, shaderFS, shaderProg[5];   // handles to objects
GLint  linked;

int vel = 1;
bool translationOn = false;
bool rotationOn = false;
bool debugOn = true;
bool backCullingOn = false;
bool frustumCullingOn = false;

double cpu_time(void)
{

	double value;
	value = (double) clock () / (double) CLOCKS_PER_SEC;
	return value;

}

void calculateFPS()
{

	frameCount++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);

    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        fps = frameCount / (timeInterval / 1000.0f);
        previousTime = currentTime;
        frameCount = 0;
	
		printf("FPS: %f\n", fps);
	}

}

void reshape(int w, int h)
{
	
	windowWidth = w;
	windowHeight = h;

}

void displayScene(GLuint shader)
{

	glViewport(0, 0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	glUseProgram(shader);
	geometryViewer.setShaderProg(shader);
	geometryViewer.setEye(cameraEye);
	geometryViewer.setLook(cameraAt);
	geometryViewer.setUp(cameraUp);
	geometryViewer.configureAmbient(windowWidth, windowHeight);
	geometryViewer.configurePhong(lightEye, cameraEye);
	
	if(backCullingOn) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	if(frustumCullingOn || debugOn) frustum.updateMesh(geometryViewer.getProjectionMatrix() * geometryViewer.getViewMatrix() * geometryViewer.getModelMatrix());

	if(debugOn) {
		
		geometryViewer.setEye(debugEye);
		geometryViewer.setLook(debugAt);
		geometryViewer.configureAmbient(windowWidth, windowHeight);
		geometryViewer.configurePhong(lightEye, debugEye);
		geometryViewer.loadVBOs(sceneVBO[scene.size()], &frustum.getMesh());
		geometryViewer.drawMesh(sceneVBO[scene.size()], frustum.getMesh().getIndicesSize(), frustum.getMesh().getTextureCoordsSize(), frustum.getMesh().getTextures(), frustum.getMesh().getColorSize());
	
	}

	for(int mesh = 0; mesh < scene.size(); mesh++) {

		bool render = true;
		if(frustumCullingOn) render = !frustum.cull(scene[mesh].getAABB());
		if(render) geometryViewer.drawMesh(sceneVBO[mesh], scene[mesh].getIndicesSize(), scene[mesh].getTextureCoordsSize(), scene[mesh].getTextures(), scene[mesh].getColorSize());
		
	}
	
	if(backCullingOn) {
		glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	glUseProgram(0);

}

void displaySceneFromGBuffer(GLuint shader)
{

	glViewport(0, 0, windowWidth, windowHeight);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shader);
	
	geometryViewer.setShaderProg(shader);
	geometryViewer.setEye(cameraEye);
	geometryViewer.setLook(cameraAt);
	geometryViewer.setUp(cameraUp);
	geometryViewer.configureAmbient(windowWidth, windowHeight);
	geometryViewer.configureGBuffer(textures[VERTEX_MAP_COLOR], textures[NORMAL_MAP_COLOR], textures[TEXTURE_MAP_COLOR]);
	geometryViewer.configurePhong(lightEye, cameraEye);
	
	textureViewer.setShaderProg(shader);
	textureViewer.drawTextureQuad();
	
	glUseProgram(0);

}

void display()
{

	//Render scene into a G-Buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[GBUFFER_FRAMEBUFFER]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayScene(shaderProg[GBUFFER_SHADER]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Perform Phong shading
	glClearColor(0.63f, 0.82f, 0.96f, 1.0);
	displaySceneFromGBuffer(shaderProg[PHONG_SHADER]);

	glutSwapBuffers();
	glutPostRedisplay();

}

void idle()
{	
	calculateFPS();
}	

void keyboard(unsigned char key, int x, int y) 
{

	switch(key) {
	case 27:
		exit(0);
		break;
	case 'c':
		std::cout << cameraEye[0] << " " << cameraEye[1] << " " << cameraEye[2] << std::endl;
		std::cout << cameraAt[0] << " " << cameraAt[1] << " " << cameraAt[2] << std::endl;
		break;
	}

}

void specialKeyboard(int key, int x, int y)
{

	float temp;
	switch(key) {
	case GLUT_KEY_UP:
		if(translationOn) {
			cameraEye[1] += vel;
			cameraAt[1] += vel;
		}
		if(rotationOn) {
			temp = cameraAt[2];
			cameraAt = glm::mat3(glm::rotate((float)5 * vel, glm::vec3(1.0, 0.0, 0.0))) * cameraAt;
			cameraAt[2] = temp;
			cameraUp = glm::mat3(glm::rotate((float)5 * vel, glm::vec3(1.0, 0.0, 0.0))) * glm::vec3(0.0, 1.0, 0.0);
		}
		break;
	case GLUT_KEY_DOWN:
		if(translationOn) {
			cameraEye[1] -= vel;
			cameraAt[1] -= vel;
		}
		if(rotationOn) {
			temp = cameraAt[2];
			cameraAt = glm::mat3(glm::rotate((float)-5 * vel, glm::vec3(1.0, 0.0, 0.0))) * cameraAt;
			cameraAt[2] = temp;
			cameraUp = glm::mat3(glm::rotate((float)-5 * vel, glm::vec3(1.0, 0.0, 0.0))) * glm::vec3(0.0, 1.0, 0.0);
		}
		break;
	case GLUT_KEY_LEFT:
		if(translationOn) {
			cameraEye[0] -= vel;
			cameraAt[0] -= vel;
		}
		if(rotationOn) {
			temp = cameraAt[2];
			cameraAt = glm::mat3(glm::rotate((float)-5 * vel, glm::vec3(0.0, 1.0, 0.0))) * cameraAt;
			cameraAt[2] = temp;
			cameraUp = glm::mat3(glm::rotate((float)-5 * vel, glm::vec3(0.0, 1.0, 0.0))) * glm::vec3(0.0, 1.0, 0.0);
		}
		break;
	case GLUT_KEY_RIGHT:
		if(translationOn) {
			cameraEye[0] += vel;
			cameraAt[0] += vel;
		}
		if(rotationOn) {
			temp = cameraAt[2];
			cameraAt = glm::mat3(glm::rotate((float)5 * vel, glm::vec3(0.0, 1.0, 0.0))) * cameraAt;
			cameraAt[2] = temp;
			cameraUp = glm::mat3(glm::rotate((float)5 * vel, glm::vec3(0.0, 1.0, 0.0))) * glm::vec3(0.0, 1.0, 0.0);
		}
		break;
	case GLUT_KEY_PAGE_UP:
		if(translationOn) {
			cameraEye[2] += vel;
			cameraAt[2] += vel;
		}
		if(rotationOn) {
			cameraAt = glm::mat3(glm::rotate((float)5 * vel, glm::vec3(0.0, 0.0, 1.0))) * cameraAt;
			cameraUp = glm::mat3(glm::rotate((float)5 * vel, glm::vec3(0.0, 0.0, 1.0))) * glm::vec3(0.0, 1.0, 0.0);
		}
		break;
	case GLUT_KEY_PAGE_DOWN:
		if(translationOn) {
			cameraEye[2] -= vel;
			cameraAt[2] -= vel;
		}
		if(rotationOn) {
			cameraAt = glm::mat3(glm::rotate((float)-5 * vel, glm::vec3(0.0, 0.0, 1.0))) * cameraAt;
			cameraUp = glm::mat3(glm::rotate((float)-5 * vel, glm::vec3(0.0, 0.0, 1.0))) * glm::vec3(0.0, 1.0, 0.0);
		}
		break;
	}

}

void transformationMenu(int id) {

	switch(id)
	{
		case 0:
			translationOn = true;
			rotationOn = false;
			break;
		case 1:
			rotationOn = true;
			translationOn = false;
			break;
	}

}

void cullingMenu(int id) {
	
	switch(id)
	{
		case 0:
			backCullingOn = false;
			frustumCullingOn = false;
			break;
		case 1:
			backCullingOn = true;
			break;
		case 2:
			frustumCullingOn = true;
			break;
	}

}

void mainMenu(int id) {

	switch(id)
	{
		case 0:
			debugOn = !debugOn;
			break;
	}

}

void createMenu() {

	GLint cullingMenuID, transformationMenuID;

	transformationMenuID = glutCreateMenu(transformationMenu);
		glutAddMenuEntry("Translation", 0);
		glutAddMenuEntry("Rotation", 1);
	
	cullingMenuID = glutCreateMenu(cullingMenu);
		glutAddMenuEntry("Disable Culling", 0);
		glutAddMenuEntry("Back Culling", 1);
		glutAddMenuEntry("Frustum Culling", 2);

	glutCreateMenu(mainMenu);
		glutAddSubMenu("Culling", cullingMenuID);
		glutAddSubMenu("Transformation", transformationMenuID);
		glutAddMenuEntry("Debugging", 0);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

}

void initGL(char *configurationFile) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  

	sceneLoader = new SceneLoader(configurationFile);
	sceneLoader->load();
	scene = sceneLoader->getScene();
	
	sceneVBO.resize(scene.size() + 1);
	for (int mesh = 0; mesh < scene.size(); mesh++) {
		sceneVBO[mesh] = (GLuint*)malloc(5 * sizeof(GLuint));
		glGenBuffers(5, sceneVBO[mesh]);
		geometryViewer.loadVBOs(sceneVBO[mesh], &scene[mesh]);
	}
	sceneVBO[scene.size()] = (GLuint*)malloc(5 * sizeof(GLuint));
	glGenBuffers(5, sceneVBO[scene.size()]);
		
	if(textures[0] == 0)
		glGenTextures(10, textures);
	if(frameBuffer[0] == 0)
		glGenFramebuffers(2, frameBuffer);
	
	cameraEye[0] = sceneLoader->getCameraPosition()[0]; cameraEye[1] = sceneLoader->getCameraPosition()[1]; cameraEye[2] = sceneLoader->getCameraPosition()[2];
	cameraAt[0] = sceneLoader->getCameraAt()[0]; cameraAt[1] = sceneLoader->getCameraAt()[1]; cameraAt[2] = sceneLoader->getCameraAt()[2];
	cameraUp[0] = 0.0; cameraUp[1] = 0.0; cameraUp[2] = 1.0;
	lightEye[0] = sceneLoader->getLightPosition()[0]; lightEye[1] = sceneLoader->getLightPosition()[1]; lightEye[2] = sceneLoader->getLightPosition()[2];
	lightAt[0] = sceneLoader->getLightAt()[0]; lightAt[1] = sceneLoader->getLightAt()[1]; lightAt[2] = sceneLoader->getLightAt()[2];
	lightUp[0] = 0.0; lightUp[1] = 0.0; lightUp[2] = 1.0;
	debugEye[0] = sceneLoader->getDebugPosition()[0]; debugEye[1] = sceneLoader->getDebugPosition()[1]; debugEye[2] = sceneLoader->getDebugPosition()[2];
	debugAt[0] = sceneLoader->getDebugAt()[0]; debugAt[1] = sceneLoader->getDebugAt()[1]; debugAt[2] = sceneLoader->getDebugAt()[2];
	debugUp[0] = 0.0; debugUp[1] = 0.0; debugUp[2] = 1.0;
	
	textureViewer.loadQuad();
	createMenu();
	
	textureViewer.loadRGBATexture((float*)NULL, textures, VERTEX_MAP_COLOR, windowWidth, windowHeight, GL_NEAREST);
	textureViewer.loadRGBATexture((float*)NULL, textures, NORMAL_MAP_COLOR, windowWidth, windowHeight, GL_NEAREST);
	textureViewer.loadRGBATexture((float*)NULL, textures, TEXTURE_MAP_COLOR, windowWidth, windowHeight, GL_NEAREST);
	
	textureViewer.loadDepthComponentTexture(NULL, textures, GBUFFER_MAP_DEPTH, windowWidth, windowHeight);
	
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[GBUFFER_FRAMEBUFFER]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textures[GBUFFER_MAP_DEPTH], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[VERTEX_MAP_COLOR], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures[NORMAL_MAP_COLOR], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures[TEXTURE_MAP_COLOR], 0);
	GLenum bufs[3];
	bufs[0] = GL_COLOR_ATTACHMENT0;
	bufs[1] = GL_COLOR_ATTACHMENT1;
	bufs[2] = GL_COLOR_ATTACHMENT2;
	glDrawBuffers(3,bufs);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void releaseGL() {

	glDeleteTextures(10, textures);
	glDeleteFramebuffers(2, frameBuffer);
	for (int i = 0; i < scene.size(); i++) glDeleteBuffers(5, sceneVBO[i]);
	
}

int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Culling");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);

	glewInit();
	initGL(argv[1]);

	initShader("Shaders/GBuffer", GBUFFER_SHADER);
	initShader("Shaders/PhongShading", PHONG_SHADER);
	glUseProgram(0); 

	glutMainLoop();

	releaseGL();
	delete sceneLoader;
	
	return 0;

}
