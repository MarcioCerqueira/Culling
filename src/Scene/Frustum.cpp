#include "Scene\Frustum.h"

Frustum::Frustum()
{

	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));
	frustumMesh.addPoint(glm::vec3(0.0));

	frustumMesh.addColor(glm::vec3(1.0, 0.0, 0.0));
	frustumMesh.addColor(glm::vec3(1.0, 0.0, 0.0));
	frustumMesh.addColor(glm::vec3(1.0, 0.0, 0.0));
	frustumMesh.addColor(glm::vec3(1.0, 0.0, 0.0));
	frustumMesh.addColor(glm::vec3(0.0, 0.0, 1.0));
	frustumMesh.addColor(glm::vec3(0.0, 0.0, 1.0));
	frustumMesh.addColor(glm::vec3(0.0, 0.0, 1.0));
	frustumMesh.addColor(glm::vec3(0.0, 0.0, 1.0));

	frustumMesh.addIndex(2); frustumMesh.addIndex(1); frustumMesh.addIndex(0);
	frustumMesh.addIndex(1); frustumMesh.addIndex(2); frustumMesh.addIndex(3);
	frustumMesh.addIndex(6); frustumMesh.addIndex(5); frustumMesh.addIndex(4);
	frustumMesh.addIndex(5); frustumMesh.addIndex(6); frustumMesh.addIndex(7);
	frustumMesh.addIndex(0); frustumMesh.addIndex(4); frustumMesh.addIndex(5);
	frustumMesh.addIndex(0); frustumMesh.addIndex(5); frustumMesh.addIndex(1);
	frustumMesh.addIndex(2); frustumMesh.addIndex(6); frustumMesh.addIndex(3);
	frustumMesh.addIndex(3); frustumMesh.addIndex(6); frustumMesh.addIndex(7);
	frustumMesh.addIndex(6); frustumMesh.addIndex(4); frustumMesh.addIndex(0);
	frustumMesh.addIndex(6); frustumMesh.addIndex(0); frustumMesh.addIndex(2);
	frustumMesh.addIndex(1); frustumMesh.addIndex(5); frustumMesh.addIndex(3);
	frustumMesh.addIndex(3); frustumMesh.addIndex(5); frustumMesh.addIndex(7);

}

void Frustum::updateMesh(glm::mat4 mvp)
{

	glm::mat4 inverseMVP = glm::inverse(mvp);

	glm::vec4 A = inverseMVP * glm::vec4(-1, 1, -0.999, 1);
	glm::vec4 B = inverseMVP * glm::vec4(1, 1, -0.999, 1);
	glm::vec4 C = inverseMVP * glm::vec4(-1, -1, -0.999, 1);
	glm::vec4 D = inverseMVP * glm::vec4(1, -1, -0.999, 1);
	glm::vec4 E = inverseMVP * glm::vec4(-1, 1, 0.999, 1);
	glm::vec4 F = inverseMVP * glm::vec4(1, 1, 0.999, 1);
	glm::vec4 G = inverseMVP * glm::vec4(-1, -1, 0.999, 1);
	glm::vec4 H = inverseMVP * glm::vec4(1, -1, 0.999, 1);
	
	frustumMesh.setPoint(glm::vec3(A/A.w), 0);
	frustumMesh.setPoint(glm::vec3(B/B.w), 1);
	frustumMesh.setPoint(glm::vec3(C/C.w), 2);
	frustumMesh.setPoint(glm::vec3(D/D.w), 3);
	frustumMesh.setPoint(glm::vec3(E/E.w), 4);
	frustumMesh.setPoint(glm::vec3(F/F.w), 5);
	frustumMesh.setPoint(glm::vec3(G/G.w), 6);
	frustumMesh.setPoint(glm::vec3(H/H.w), 7);
	
	frustumMesh.computeNormals();
	
}

bool Frustum::cull(AABB boundingBox, bool planeCulling) 
{

	if(planeCulling) {
	
		planes[0].setPoints(frustumMesh.getPoint(1), frustumMesh.getPoint(0), frustumMesh.getPoint(4));
		planes[1].setPoints(frustumMesh.getPoint(2), frustumMesh.getPoint(3), frustumMesh.getPoint(7));
		planes[2].setPoints(frustumMesh.getPoint(0), frustumMesh.getPoint(2), frustumMesh.getPoint(6));
		planes[3].setPoints(frustumMesh.getPoint(3), frustumMesh.getPoint(1), frustumMesh.getPoint(7));
		planes[4].setPoints(frustumMesh.getPoint(0), frustumMesh.getPoint(1), frustumMesh.getPoint(3));
		planes[5].setPoints(frustumMesh.getPoint(5), frustumMesh.getPoint(4), frustumMesh.getPoint(6));
	
		for(int plane = 0; plane < 6; plane++) {
			
			glm::vec3 p = glm::vec3(boundingBox.minX.x, boundingBox.minY.y, boundingBox.minZ.z);
			if(planes[plane].getNormal().x >= 0) p.x = boundingBox.maxX.x;
			if(planes[plane].getNormal().y >= 0) p.y = boundingBox.maxY.y;
			if(planes[plane].getNormal().z >= 0) p.z = boundingBox.maxZ.z;

			glm::vec3 n = glm::vec3(boundingBox.maxX.x, boundingBox.maxY.y, boundingBox.maxZ.z);
			if(planes[plane].getNormal().x >= 0) n.x = boundingBox.minX.x;
			if(planes[plane].getNormal().y >= 0) n.y = boundingBox.minY.y;
			if(planes[plane].getNormal().z >= 0) n.z = boundingBox.minZ.z;

			if(planes[plane].distance(p) < 0) return true;
			
		}
		return false;
		
	} else {

		frustumMesh.computeAABB();
		if(boundingBox.maxX.x < frustumMesh.getAABB().minX.x || boundingBox.minX.x > frustumMesh.getAABB().maxX.x ||
		boundingBox.maxY.y < frustumMesh.getAABB().minY.y || boundingBox.minY.y > frustumMesh.getAABB().maxY.y ||
		boundingBox.maxZ.z < frustumMesh.getAABB().minZ.z || boundingBox.minZ.z > frustumMesh.getAABB().maxZ.z) return true;
		else return false;
	
	}
	

}

bool Frustum::cull(Sphere sphere, bool planeCulling) 
{
	
	if(planeCulling) {

		planes[0].setPoints(frustumMesh.getPoint(1), frustumMesh.getPoint(0), frustumMesh.getPoint(4));
		planes[1].setPoints(frustumMesh.getPoint(2), frustumMesh.getPoint(3), frustumMesh.getPoint(7));
		planes[2].setPoints(frustumMesh.getPoint(0), frustumMesh.getPoint(2), frustumMesh.getPoint(6));
		planes[3].setPoints(frustumMesh.getPoint(3), frustumMesh.getPoint(1), frustumMesh.getPoint(7));
		planes[4].setPoints(frustumMesh.getPoint(0), frustumMesh.getPoint(1), frustumMesh.getPoint(3));
		planes[5].setPoints(frustumMesh.getPoint(5), frustumMesh.getPoint(4), frustumMesh.getPoint(6));
		
		for(int plane = 0; plane < 6; plane++) if(planes[plane].distance(sphere.center) < -sphere.radius) return true;
		return false;

	} else {

		frustumMesh.computeAABB();
		if(sphere.center.x - sphere.radius < frustumMesh.getAABB().minX.x && sphere.center.x + sphere.radius < frustumMesh.getAABB().minX.x) return true;
		else if(sphere.center.x - sphere.radius > frustumMesh.getAABB().maxX.x && sphere.center.x + sphere.radius > frustumMesh.getAABB().maxX.x) return true;
		else if(sphere.center.y - sphere.radius < frustumMesh.getAABB().minY.y && sphere.center.y + sphere.radius < frustumMesh.getAABB().minY.y) return true;
		else if(sphere.center.y - sphere.radius > frustumMesh.getAABB().maxY.y && sphere.center.y + sphere.radius > frustumMesh.getAABB().maxY.y) return true;
		else if(sphere.center.z - sphere.radius < frustumMesh.getAABB().minZ.z && sphere.center.z + sphere.radius < frustumMesh.getAABB().minZ.z) return true;
		else if(sphere.center.z - sphere.radius > frustumMesh.getAABB().maxZ.z && sphere.center.z + sphere.radius > frustumMesh.getAABB().maxZ.z) return true;
		else return false;
	
	}
	
}