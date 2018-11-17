#include "ResourceMesh.h"

#include "Application.h"
#include "ModuleGOs.h"
#include "SceneImporter.h"

#include "glew\include\GL\glew.h"

ResourceMesh::ResourceMesh(ResourceType type, uint uuid) : Resource(type, uuid) {}

ResourceMesh::~ResourceMesh() 
{
	App->GOs->InvalidateResource(this);
}

bool ResourceMesh::LoadInMemory()
{
	bool ret = App->sceneImporter->Load(exportedFile.data(), this);

	if (!ret)
		return ret;

	glGenBuffers(1, (GLuint*)&verticesID);
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*)&indicesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indicesSize, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*)&textureCoordsID);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize * 2, textureCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return ret;
}

bool ResourceMesh::UnloadFromMemory()
{
	glDeleteBuffers(1, (GLuint*)&verticesID);
	glDeleteBuffers(1, (GLuint*)&indicesID);
	glDeleteBuffers(1, (GLuint*)&textureCoordsID);

	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(textureCoords);

	return true;
}
