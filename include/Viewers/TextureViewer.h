#ifndef MYGLTEXTUREVIEWER_H
#define MYGLTEXTUREVIEWER_H


#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>

class TextureViewer
{

public:
	void loadDepthComponentTexture(float *data, GLuint *texVBO, int index, int imageWidth, int imageHeight);
	void loadRGBTexture(const unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight);
	void loadRGBTexture(float *data, GLuint *texVBO, int index, int imageWidth, int imageHeight, GLint param = GL_LINEAR_MIPMAP_LINEAR);
	void loadRGBATexture(float *data, GLuint *texVBO, int index, int imageWidth, int imageHeight, GLint param = GL_LINEAR_MIPMAP_LINEAR);
	void loadRGBATexture(int *data, GLuint *texVBO, int index, int imageWidth, int imageHeight, GLint param = GL_LINEAR_MIPMAP_LINEAR);
	void loadFrameBufferTexture(int x, int y, int width, int height, unsigned char *frameBuffer);
	void loadQuad();
	void drawTextureQuad();
	void drawTextureOnShader(GLuint texture, int imageWidth, int imageHeight);
	void sendData(GLuint shaderProg) {
		GLuint texLoc = glGetUniformLocation(shaderProg, "model");
		glUniformMatrix4fv(texLoc, 1, GL_FALSE, &lightModelView[0]);
	}
	void setShaderProg(GLuint shaderProg) { this->shaderProg = shaderProg; }
	
private:
	GLfloat lightModelView[16];
	GLfloat lightProjection[16];
	GLuint shaderProg;
	GLuint vbo_cube_texcoords;
	GLuint ibo_cube_elements;
};

#endif