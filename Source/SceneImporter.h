#include "GameMode.h"

#ifndef __SCENE_IMPORTER_H__
#define __SCENE_IMPORTER_H__

#include "Importer.h"

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/version.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc140-mt.lib")

#include <vector>

struct Mesh
{
	// Unique vertices
	float* vertices = nullptr;
	uint verticesSize = 0;
	uint verticesID = 0;

	// Indices
	uint* indices = nullptr;
	uint indicesID = 0;
	uint indicesSize = 0;

	// Texture Coords
	float* textureCoords = nullptr;
	uint textureCoordsID = 0;
	uint textureCoordsSize = 0;

	void Init();
	~Mesh();
};

struct ModelImportSettings
{
	
};

struct aiScene;
struct aiNode;

class GameObject;
class ResourceMesh;

class SceneImporter : public Importer
{
public:

	SceneImporter();
	~SceneImporter();

	bool Import(const char* importFileName, const char* importPath, std::string& outputFileName);
	bool Import(const void* buffer, uint size, std::string& outputFileName);
	void RecursivelyImportNodes(const aiScene* scene, const aiNode* node, const GameObject* parentGO, GameObject* transformationGO, std::string& outputFileName);

	void GenerateMeta(Resource* resource);

	bool Load(const char* exportedFileName, Mesh* outputMesh);
	bool Load(const void* buffer, uint size, Mesh* outputMesh);

	uint GetAssimpMajorVersion() const;
	uint GetAssimpMinorVersion() const;
	uint GetAssimpRevisionVersion() const;

public:

	// Default import values
	ModelImportSettings defaultImportSettings;
};

#endif