#include "Viewers\GeometryViewer.h"

GeometryViewer::GeometryViewer()
{

	fov = 45.f;
	zNear = 1.0f;
	zFar = 300.0f;
	normalMatrix = glm::mat3(1.0);

}

void GeometryViewer::configureAmbient(int windowWidth, int windowHeight)
{
	
	projection = glm::perspective(fov, (GLfloat)windowWidth/windowHeight, zNear, zFar);
	view = glm::lookAt(eye, look, up);
	model = glm::mat4(1.0f);

	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
}

void GeometryViewer::configureGBuffer(GLuint vertexTexture, GLuint normalTexture, GLuint colorTexture) {

	GLuint vertexMap = glGetUniformLocation(shaderProg, "vertexMap");
	glUniform1i(vertexMap, 8);
	GLuint normalMap = glGetUniformLocation(shaderProg, "normalMap");
	glUniform1i(normalMap, 9);
	GLuint colorMap = glGetUniformLocation(shaderProg, "colorMap");
	glUniform1i(colorMap, 10);
	
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, vertexTexture);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	
	glActiveTexture(GL_TEXTURE8);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE9);
	glDisable(GL_TEXTURE_2D);
		
	glActiveTexture(GL_TEXTURE10);
	glDisable(GL_TEXTURE_2D);

}

void GeometryViewer::configureLight() {
	
	GLfloat ambient[4] = {0.1, 0.1, 0.1, 1.0};
	GLfloat diffuse[4] = {0.9, 0.9, 0.9, 1.0};
	GLfloat specular[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat position[4] = {10.0, 40.0, 0.0, 1.0};

	GLfloat specularity[4] = {1.0, 1.0, 1.0, 1.0};
	GLint shininess = 60;

	glClearColor(0.0, 0, 0, 1);
	
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT,GL_SPECULAR, specularity);
	glMateriali(GL_FRONT,GL_SHININESS, shininess);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

}

void GeometryViewer::configureLinearization()
{

	GLuint zNearID = glGetUniformLocation(shaderProg, "zNear");
	glUniform1i(zNearID, zNear);
	GLuint zFarID = glGetUniformLocation(shaderProg, "zFar");
	glUniform1i(zFarID, zFar);
	GLuint fovID = glGetUniformLocation(shaderProg, "fov");
	glUniform1f(fovID, fov);
	
}

void GeometryViewer::configurePhong(glm::vec3 lightPosition, glm::vec3 cameraPosition) 
{

	glm::mat4 mvp = projection * view * model;
	glm::mat4 mv = view * model;

	if(!hasNormalMatrixBeenSet) {
		normalMatrix = glm::inverseTranspose(glm::mat3(mv));
		hasNormalMatrixBeenSet = true;
	} 

	GLuint mvpId = glGetUniformLocation(shaderProg, "MVP");
	glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
	GLuint inverseMVPID = glGetUniformLocation(shaderProg, "inverseMVP");
	glUniformMatrix4fv(inverseMVPID, 1, GL_FALSE, &glm::inverse(mvp)[0][0]);
	GLuint mvId = glGetUniformLocation(shaderProg, "MV");
	glUniformMatrix4fv(mvId, 1, GL_FALSE, &mv[0][0]);
	GLuint nMId = glGetUniformLocation(shaderProg, "normalMatrix");
	glUniformMatrix3fv(nMId, 1, GL_FALSE, &normalMatrix[0][0]);
	GLuint lightId = glGetUniformLocation(shaderProg, "lightPosition");
	glUniform3f(lightId, lightPosition[0], lightPosition[1], lightPosition[2]);
	GLuint cameraLightId = glGetUniformLocation(shaderProg, "cameraPosition");
	glUniform3f(cameraLightId, cameraPosition[0], cameraPosition[1], cameraPosition[2]);

}

void GeometryViewer::drawPlane(float x, float y, float z) {
	
	glBegin(GL_QUADS);
	glVertex3f(-x, 0.0, -z);
	glVertex3f(+x, 0.0, -z);
	glVertex3f(+x, 0.0, +z);
	glVertex3f(-x, 0.0, +z);
	glEnd();

}

void GeometryViewer::drawMesh(GLuint *VBOs, int numberOfIndices, int numberOfTexCoords, std::vector<Texture> textures, int numberOfColors)
{
	
	GLuint useTextureID = glGetUniformLocation(shaderProg, "useTexture");
	glUniform1i(useTextureID, textures.size() > 0 ? 1 : 0);
	
	GLuint colorID = glGetUniformLocation(shaderProg, "useMeshColor");
	glUniform1i(colorID, numberOfColors > 0 ? 1 : 0);
	

	if(textures.size() > 0) {
		
		unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            
			std::string number;
            std::string name = textures[i].type;
        
			if(name == "texture_diffuse") number = std::to_string((long double)diffuseNr++);
			else if(name == "texture_specular") number = std::to_string((long double)specularNr++); // transfer unsigned int to stream
           
			glUniform1i(glGetUniformLocation(shaderProg, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
	
	}
	
	GLint attribute_vert = glGetAttribLocation(shaderProg, "vertex");
	glEnableVertexAttribArray(attribute_vert);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glVertexAttribPointer(attribute_vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLint attribute_norm = glGetAttribLocation(shaderProg, "normal");
	glEnableVertexAttribArray(attribute_norm);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glVertexAttribPointer(attribute_norm, 3, GL_FLOAT, GL_TRUE, 0, 0);
	
	GLint attribute_uv; 
	if(numberOfTexCoords > 0) {

		attribute_uv = glGetAttribLocation(shaderProg, "uv");
		glEnableVertexAttribArray(attribute_uv);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	}
	
	GLint attribute_color;
	if(numberOfColors > 0) {
		attribute_color = glGetAttribLocation(shaderProg, "color");
		glEnableVertexAttribArray(attribute_color);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
		glVertexAttribPointer(attribute_color, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[4]);
	glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(attribute_vert);
	glDisableVertexAttribArray(attribute_norm);
	if(numberOfTexCoords > 0) glDisableVertexAttribArray(attribute_uv);
	if(numberOfColors > 0) glDisableVertexAttribArray(attribute_color);

	if(textures.size() > 0) {
		
		for(unsigned int i = 0; i < textures.size(); i++) {
        	
			glActiveTexture(GL_TEXTURE2 + i);
			glDisable(GL_TEXTURE_2D);
		
		}

	}
	
}

void GeometryViewer::loadVBOs(GLuint *VBOs, Mesh *scene, bool drawBoundingBox)
{

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	if(drawBoundingBox) glBufferData(GL_ARRAY_BUFFER, scene->getAABB().vertices.size() * 3 * sizeof(float), &scene->getAABB().vertices[0], GL_DYNAMIC_DRAW);
	else glBufferData(GL_ARRAY_BUFFER, scene->getPointCloudSize() * 3 * sizeof(float), scene->getPointCloud(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	if(drawBoundingBox) glBufferData(GL_ARRAY_BUFFER, scene->getAABB().normals.size() * 3 * sizeof(float), &scene->getAABB().normals[0], GL_DYNAMIC_DRAW);
	else glBufferData(GL_ARRAY_BUFFER, scene->getPointCloudSize() * 3 * sizeof(float), scene->getNormalVector(), GL_DYNAMIC_DRAW);
	
	if(scene->getTextureCoordsSize() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glBufferData(GL_ARRAY_BUFFER, scene->getTextureCoordsSize() * 2 * sizeof(float), scene->getTextureCoordinates(), GL_DYNAMIC_DRAW);
	}
	
	if(scene->getColorSize() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
		glBufferData(GL_ARRAY_BUFFER, scene->getColorSize() * 3 * sizeof(float), scene->getColors(), GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[4]);
	if(drawBoundingBox) glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene->getAABB().indices.size() * sizeof(int), &scene->getAABB().indices[0], GL_DYNAMIC_DRAW);
	else glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene->getIndicesSize() * sizeof(int), scene->getIndices(), GL_DYNAMIC_DRAW);

}