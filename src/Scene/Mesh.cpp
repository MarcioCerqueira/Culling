#include "Scene\Mesh.h"

Mesh::Mesh() 
{
}

Mesh::~Mesh()
{
}

void Mesh::computeNormals()
{

	if(normals.size() == 0) {

		std::vector <int> nb_seen;
		nb_seen.resize(vertices.size());
		normals.resize(vertices.size());

		for(int i = 0; i < indices.size(); i+=3) {
			int a = indices[i + 0];
			int b = indices[i + 1];
			int c = indices[i + 2];
			glm::vec3 normal = glm::normalize(glm::cross(
				glm::vec3(vertices[b].x, vertices[b].y, vertices[b].z) - glm::vec3(vertices[a].x, vertices[a].y, vertices[a].z),
				glm::vec3(vertices[c].x, vertices[c].y, vertices[c].z) - glm::vec3(vertices[a].x, vertices[a].y, vertices[a].z)));

			int v[3];  v[0] = a;  v[1] = b;  v[2] = c;
			for(int j = 0; j < 3; j++) {
			
				int cur_v = v[j];
				nb_seen[cur_v]++;
				if(nb_seen[cur_v] == 1) {
					normals[cur_v] = normal;
				} else {
					normals[cur_v].x = normals[cur_v].x * (1.0 - 1.0/nb_seen[cur_v]) + normal.x * 1.0/nb_seen[cur_v];
					normals[cur_v].y = normals[cur_v].y * (1.0 - 1.0/nb_seen[cur_v]) + normal.y * 1.0/nb_seen[cur_v];
					normals[cur_v].z = normals[cur_v].z * (1.0 - 1.0/nb_seen[cur_v]) + normal.z * 1.0/nb_seen[cur_v];
				}

			}		

		}

	}

}

void Mesh::computeAABB() {
	
	boundingBox.minX = vertices[0];
	boundingBox.minY = vertices[0];
	boundingBox.minZ = vertices[0];
	boundingBox.maxX = vertices[0];
	boundingBox.maxY = vertices[0];
	boundingBox.maxZ = vertices[0];
	
	for(int point = 1; point < vertices.size(); point++) {
		
		if(vertices[point].x < boundingBox.minX.x) boundingBox.minX = vertices[point];
		if(vertices[point].y < boundingBox.minY.y) boundingBox.minY = vertices[point];
		if(vertices[point].z < boundingBox.minZ.z) boundingBox.minZ = vertices[point];
		if(vertices[point].x > boundingBox.maxX.x) boundingBox.maxX = vertices[point];
		if(vertices[point].y > boundingBox.maxY.y) boundingBox.maxY = vertices[point];
		if(vertices[point].z > boundingBox.maxZ.z) boundingBox.maxZ = vertices[point];

	}

}

void Mesh::computeSphere() {
	
	sphere.center = glm::vec3(0.0f);
	sphere.radius = 0.0;

	for(int point = 0; point < vertices.size(); point++) sphere.center += vertices[point];
	sphere.center /= vertices.size();
	for(int point = 0; point < vertices.size(); point++) {
		float distance = sqrtf(powf(sphere.center.x - vertices[point].x, 2) + powf(sphere.center.y - vertices[point].y, 2) + powf(sphere.center.z - vertices[point].z, 2));
		if(distance > sphere.radius) sphere.radius = distance;
	}
	
}

void Mesh::convertAABBToMesh() {
	
	boundingBox.vertices.push_back(glm::vec3(boundingBox.minX.x, boundingBox.minY.y, boundingBox.minZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.minX.x, boundingBox.minY.y, boundingBox.maxZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.minX.x, boundingBox.maxY.y, boundingBox.minZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.minX.x, boundingBox.maxY.y, boundingBox.maxZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.maxX.x, boundingBox.minY.y, boundingBox.minZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.maxX.x, boundingBox.minY.y, boundingBox.maxZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.maxX.x, boundingBox.maxY.y, boundingBox.minZ.z));
	boundingBox.vertices.push_back(glm::vec3(boundingBox.maxX.x, boundingBox.maxY.y, boundingBox.maxZ.z));
	
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	boundingBox.normals.push_back(glm::vec3(1.0, 1.0, 1.0));
	
	boundingBox.indices.resize(36);
	boundingBox.indices[0] = 2; boundingBox.indices[1] = 4; boundingBox.indices[2] = 0;
	boundingBox.indices[3] = 2; boundingBox.indices[4] = 6; boundingBox.indices[5] = 4;
	boundingBox.indices[6] = 2; boundingBox.indices[7] = 0; boundingBox.indices[8] = 1;
	boundingBox.indices[9] = 2; boundingBox.indices[10] = 1; boundingBox.indices[11] = 3;
	boundingBox.indices[12] = 6; boundingBox.indices[13] = 5; boundingBox.indices[14] = 4;
	boundingBox.indices[15] = 6; boundingBox.indices[16] = 7; boundingBox.indices[17] = 5;
	boundingBox.indices[18] = 1; boundingBox.indices[19] = 0; boundingBox.indices[20] = 4;
	boundingBox.indices[21] = 1; boundingBox.indices[22] = 4; boundingBox.indices[23] = 5;
	boundingBox.indices[24] = 3; boundingBox.indices[25] = 2; boundingBox.indices[26] = 6;
	boundingBox.indices[27] = 3; boundingBox.indices[28] = 6; boundingBox.indices[29] = 7;
	boundingBox.indices[30] = 3; boundingBox.indices[31] = 5; boundingBox.indices[32] = 1;
	boundingBox.indices[33] = 3; boundingBox.indices[34] = 7; boundingBox.indices[35] = 5;


}

void Mesh::translate(float x, float y, float z) {

	for(int point = 0; point < vertices.size(); point++) {
		vertices[point].x += x;
		vertices[point].y += y;
		vertices[point].z += z;
	}

}

void Mesh::scale(float x, float y, float z) {
	
	for(int point = 0; point < vertices.size(); point++) {
		vertices[point].x *= x;
		vertices[point].y *= y;
		vertices[point].z *= z;
	}

}

void Mesh::rotate(float x, float y, float z) {

	glm::mat4 rotationMatrix = glm::rotate(x, glm::vec3(1, 0, 0));
	rotationMatrix *= glm::rotate(y, glm::vec3(0, 1, 0));
	rotationMatrix *= glm::rotate(z, glm::vec3(0, 0, 1));
	rotationMatrix = glm::transpose(rotationMatrix);

	float rotX, rotY, rotZ;
	for(int point = 0; point < vertices.size(); point++) {
		
		rotX = vertices[point].x * rotationMatrix[0][0] + vertices[point].y * rotationMatrix[0][1] + vertices[point].z * rotationMatrix[0][2];
		rotY = vertices[point].x * rotationMatrix[1][0] + vertices[point].y * rotationMatrix[1][1] + vertices[point].z * rotationMatrix[1][2];
		rotZ = vertices[point].x * rotationMatrix[2][0] + vertices[point].y * rotationMatrix[2][1] + vertices[point].z * rotationMatrix[2][2];
		
		vertices[point].x = rotX;
		vertices[point].y = rotY;
		vertices[point].z = rotZ;
		
		rotX = normals[point].x * rotationMatrix[0][0] + normals[point].y * rotationMatrix[0][1] + normals[point].z * rotationMatrix[0][2];
		rotY = normals[point].x * rotationMatrix[1][0] + normals[point].y * rotationMatrix[1][1] + normals[point].z * rotationMatrix[1][2];
		rotZ = normals[point].x * rotationMatrix[2][0] + normals[point].y * rotationMatrix[2][1] + normals[point].z * rotationMatrix[2][2];
		
		normals[point].x = rotX;
		normals[point].y = rotY;
		normals[point].z = rotZ;

	}

}