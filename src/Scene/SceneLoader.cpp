#include "Scene\SceneLoader.h"

SceneLoader::SceneLoader(char *filename) 
{

	this->file = std::fstream(filename);

}

void SceneLoader::load()
{

	std::string line, key, value;
	float scale[3];
	float translate[3];
	float rotate[3];
	float color[3];
	int numberOfTextures = 0;

	while(!file.eof()) 
	{

		std::getline(file, line);
		std::istringstream split(line);
		split >> key;

		if(key[0] == 'o') {
			split >> value;
			load(value);
		} else if(key[0] == 's') {
			for(int axis = 0; axis < 3; axis++) {
				split >> value;
				scale[axis] = atof(value.c_str());
			}
			for(int i = 0; i < meshes.size(); i++) meshes[i].scale(scale[0], scale[1], scale[2]);
		} else if(key[0] == 't') {
			for(int axis = 0; axis < 3; axis++) {
				split >> value;
				translate[axis] = atof(value.c_str());
			}
			for(int i = 0; i < meshes.size(); i++) meshes[i].translate(translate[0], translate[1], translate[2]);
		} else if(key[0] == 'r') {
			for(int axis = 0; axis < 3; axis++) {
				split >> value;
				rotate[axis] = atof(value.c_str());
			}
			for(int i = 0; i < meshes.size(); i++) meshes[i].rotate(rotate[0], rotate[1], rotate[2]);
		} else if(key[0] == 'v') {
			for(int axis = 0; axis < 3; axis++) {
				split >> value;
				if(key[1] == 'e') cameraPosition[axis] = atof(value.c_str());
				else cameraAt[axis] = atof(value.c_str());
			}
		} else if(key[0] == 'l') {
			for(int axis = 0; axis < 3; axis++) {
				split >> value;
				if(key[1] == 'e') lightPosition[axis] = atof(value.c_str());
				else lightAt[axis] = atof(value.c_str());
			}
		} else if(key[0] == 'd') {
			for(int axis = 0; axis < 3; axis++) {
				split >> value;
				if(key[1] == 'e') debugPosition[axis] = atof(value.c_str());
				else debugAt[axis] = atof(value.c_str());
			}
		}
	}

	for(int i = 0; i < meshes.size(); i++) {
		meshes[i].computeAABB();
		meshes[i].computeSphere();
		meshes[i].convertAABBToMesh();
	}

}

void SceneLoader::load(std::string path) {
	
	Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }   
	
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

}

void SceneLoader::processNode(aiNode *node, const aiScene *scene)
{
	
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
    }
    
	for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}


Mesh SceneLoader::processMesh(aiMesh *mesh, const aiScene *scene)
{

	Mesh sceneMesh;
	
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {        
		sceneMesh.addPoint(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		if(mesh->mNormals) sceneMesh.addNormal(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		if(mesh->mTextureCoords[0]) sceneMesh.addTextureCoordinates(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));	
	}

	 for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
    	for(unsigned int j = 0; j < face.mNumIndices; j++) sceneMesh.addIndex(face.mIndices[j]);
    }
    
	sceneMesh.computeNormals();
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    sceneMesh.addTexture(diffuseMaps);
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    sceneMesh.addTexture(specularMaps);
    
	return sceneMesh;

}

std::vector<Texture> SceneLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    
	std::vector<Texture> textures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < texturesLoaded.size(); j++)
        {
            if(std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(texturesLoaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = loadTextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            texturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    
	return textures;

}

unsigned int SceneLoader::loadTextureFromFile(const char *path, const std::string &directory)
{
    
	Image *image;
	std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
	image = new Image((char*)filename.c_str());
	width = image->getWidth();
	height = image->getHeight();
	nrComponents = image->getNumberOfChannels();

	if(image->getData())
    {

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image->getData());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		//delete [] image;
   
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

	 return textureID;
}