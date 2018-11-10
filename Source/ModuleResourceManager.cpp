#include "ModuleResourceManager.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

#include "Application.h"

#include <assert.h> 

ModuleResourceManager::ModuleResourceManager() {}

ModuleResourceManager::~ModuleResourceManager() {}

bool ModuleResourceManager::Start()
{
	// Create the resources
	std::string path;
	RecursiveCreateResourcesFromFilesInAssets("Assets", path);

	return true;
}

update_status ModuleResourceManager::Update()
{
	timer += App->timeManager->GetRealDt();
	resources;
	if (timer >= assetsCheckTime)
	{
		std::string newFileInAssets;
		if (RecursiveFindNewFileInAssets("Assets", newFileInAssets))
			ImportFile(newFileInAssets.data());

		timer = 0.0f;
	}

	return UPDATE_CONTINUE;
}

bool ModuleResourceManager::CleanUp()
{
	assert(SomethingOnMemory() == false && "Memory still allocated on vram. Code better!");

	DestroyResources();

	return true;
}

void ModuleResourceManager::SetAssetsCheckTime(float assetsCheckTime)
{
	this->assetsCheckTime = assetsCheckTime;

	if (this->assetsCheckTime > MAX_ASSETS_CHECK_TIME)
		this->assetsCheckTime = MAX_ASSETS_CHECK_TIME;
}

float ModuleResourceManager::GetAssetsCheckTime() const
{
	return assetsCheckTime;
}

void ModuleResourceManager::RecursiveCreateResourcesFromFilesInAssets(const char* dir, std::string& path)
{
	path.append(dir);
	path.append("/");

	const char** files = App->filesystem->GetFilesFromDir(dir);
	const char** it;

	for (it = files; *it != nullptr; ++it)
	{
		if (App->filesystem->IsDirectory(*it))
		{
			RecursiveCreateResourcesFromFilesInAssets(*it, path);

			uint found = path.rfind(*it);
			if (found != std::string::npos)
				path = path.substr(0, found);
		}
		else
		{
			std::string extension;
			App->filesystem->GetExtension(*it, extension);

			// Ignore scenes and metas
			if (strcmp(extension.data(), EXTENSION_SCENE) == 0
				|| strcmp(extension.data(), ".meta") == 0 || strcmp(extension.data(), ".META") == 0)
				continue;

			// Search for the meta associated to the file
			char metaFile[DEFAULT_BUF_SIZE];
			strcpy_s(metaFile, strlen(path.data()) + 1, path.data()); // path
			strcat_s(metaFile, strlen(metaFile) + strlen(*it) + 1, *it); // fileName
			const char metaExtension[] = ".meta";
			strcat_s(metaFile, strlen(metaFile) + strlen(metaExtension) + 1, metaExtension); // extension

			std::string file = path;

			// CASE 1 (file). The file has no meta associated (the file is new)
			if (!App->filesystem->Exists(metaFile))
			{
				// Import the file (using the default import settings)
				CONSOLE_LOG("FILE SYSTEM: There is a new file '%s' in %s that needs to be imported", *it, path.data());
				file.append(*it);
				ImportFile(file.data());
			}
			else
			{
				bool exists = true;

				if (strcmp(extension.data(), EXTENSION_MESH) == 0)
				{
					std::list<uint> UUIDs;
					if (App->sceneImporter->GetMeshesUUIDsFromMeta(metaFile, UUIDs))
					{
						for (std::list<uint>::const_iterator it = UUIDs.begin(); it != UUIDs.end(); ++it)
						{
							// Build the path
							char path[DEFAULT_BUF_SIZE];
							sprintf_s(path, "%u%s", *it, extension);

							exists = App->filesystem->Exists(path);

							if (!exists)
								break;
						}
					}
				}
				else if (strcmp(extension.data(), EXTENSION_TEXTURE) == 0)
				{
					uint UUID = 0;
					if (App->materialImporter->GetTextureUUIDFromMeta(metaFile, UUID))
					{
						// Build the path
						char path[DEFAULT_BUF_SIZE];
						sprintf_s(path, "%u%s", UUID, extension);

						exists = App->filesystem->Exists(path);
					}
				}

				// CASE 2 (file + meta). The file(s) in Libray associated to the meta do(es)n't exist
				if (!exists)
				{
					// Reimport the file (using the import settings from the meta)
					CONSOLE_LOG("FILE SYSTEM: There is a file '%s' in %s which Library file(s) need(s) to be reimported", *it, path.data());
					file.append(*it);
					ImportFile(file.data(), metaFile);
				}
				// CASE 3 (file + meta + Library file(s)). The resource(s) do(es)n't exist
				else
				{
					// Create the resources
					CONSOLE_LOG("FILE SYSTEM: There is a file '%s' in %s which resources need to be created", *it, path.data());
					file.append(*it);
					ImportFile(file.data(), metaFile, false);
				}
			}
		}
	}
}

bool ModuleResourceManager::RecursiveFindNewFileInAssets(const char* dir, std::string& newFileInAssets) const
{
	bool ret = false;

	newFileInAssets.append(dir);
	newFileInAssets.append("/");

	const char** files = App->filesystem->GetFilesFromDir(dir);
	const char** it;

	for (it = files; *it != nullptr; ++it)
	{
		if (App->filesystem->IsDirectory(*it))
		{
			ret = RecursiveFindNewFileInAssets(*it, newFileInAssets);

			if (!ret)
			{
				uint found = newFileInAssets.rfind(*it);
				if (found != std::string::npos)
					newFileInAssets = newFileInAssets.substr(0, found);
			}
		}
		else
		{
			std::string extension;
			App->filesystem->GetExtension(*it, extension);

			// Ignore scenes and metas
			if (strcmp(extension.data(), EXTENSION_SCENE) == 0
				|| strcmp(extension.data(), ".meta") == 0 || strcmp(extension.data(), ".META") == 0)
				continue;

			// Search for the meta associated to the file
			char metaFile[DEFAULT_BUF_SIZE];
			strcpy_s(metaFile, strlen(newFileInAssets.data()) + 1, newFileInAssets.data()); // path
			strcat_s(metaFile, strlen(metaFile) + strlen(*it) + 1, *it); // fileName
			const char metaExtension[] = ".meta";
			strcat_s(metaFile, strlen(metaFile) + strlen(metaExtension) + 1, metaExtension); // extension

			// If the file has no meta associated, then the file is new
			if (!App->filesystem->Exists(metaFile))
			{
				// Import the file
				CONSOLE_LOG("FILE SYSTEM: There is a new file '%s' in %s that needs to be imported", *it, newFileInAssets.data());
				newFileInAssets.append(*it);
				ret = true;
			}
			else
			{
				bool exists = true;

				if (strcmp(extension.data(), EXTENSION_MESH) == 0)
				{
					std::list<uint> UUIDs;
					if (App->sceneImporter->GetMeshesUUIDsFromMeta(metaFile, UUIDs))
					{
						for (std::list<uint>::const_iterator it = UUIDs.begin(); it != UUIDs.end(); ++it)
						{
							// Build the path
							char path[DEFAULT_BUF_SIZE];
							sprintf_s(path, "%u%s", *it, extension);

							exists = App->filesystem->Exists(path);

							if (!exists)
								break;
						}
					}
				}
				else if (strcmp(extension.data(), EXTENSION_TEXTURE) == 0)
				{
					uint UUID = 0;
					if (App->materialImporter->GetTextureUUIDFromMeta(metaFile, UUID))
					{
						// Build the path
						char path[DEFAULT_BUF_SIZE];
						sprintf_s(path, "%u%s", UUID, extension);

						exists = App->filesystem->Exists(path);
					}
				}

				// If the resource(s) do(es)n't exist, reimport the file
				if (!exists)
				{
					CONSOLE_LOG("FILE SYSTEM: There is a file '%s' in %s which resources need to be reimported", *it, newFileInAssets.data());
					newFileInAssets.append(*it);
				}
			}
		}

		if (ret)
			break;
	}

	return ret;
}

// Returns the UUID associated to the resource of the file. In case of error, it returns 0
uint ModuleResourceManager::Find(const char* fileInAssets) const
{
	for (auto it = resources.begin(); it != resources.end(); ++it)
	{
		if (strcmp(it->second->GetExportedFile(), fileInAssets) == 0)
			return it->first;
	}

	return 0;
}

// Imports a file into a resource. If case of success, it returns the UUID of the resource. Otherwise, it returns 0

uint ModuleResourceManager::ImportFile(const char* fileInAssets, const char* metaFile, bool import)
{
	uint ret = 0;

	if (fileInAssets == nullptr)
		return ret;

	bool imported = false;
	ImportSettings* importSettings = nullptr;
	std::string outputFileName;

	std::string extension;
	App->filesystem->GetExtension(fileInAssets, extension);
	ResourceType type = GetResourceTypeByExtension(extension.data());

	if (import)
	{
		// Initialize the import settings to the default import settings
		switch (type)
		{
		case ResourceType::Mesh_Resource:
			importSettings = new MeshImportSettings();
			break;
		case ResourceType::Texture_Resource:
			importSettings = new TextureImportSettings();
			break;
		case ResourceType::No_Type_Resource:
			break;
		}

		// If the file has a meta associated, use the import settings from the meta
		if (metaFile != nullptr)
		{
			switch (type)
			{
			case ResourceType::Mesh_Resource:
				App->sceneImporter->GetMeshImportSettingsFromMeta(metaFile, (MeshImportSettings*)importSettings);
				break;
			case ResourceType::Texture_Resource:
				App->materialImporter->GetTextureImportSettingsFromMeta(metaFile, (TextureImportSettings*)importSettings);
				break;
			case ResourceType::No_Type_Resource:
				break;
			}
		}

		// Import the file using the import settings
		switch (type)
		{
		case ResourceType::Mesh_Resource:
			imported = App->sceneImporter->Import(nullptr, fileInAssets, outputFileName, importSettings);
			break;
		case ResourceType::Texture_Resource:
			imported = App->materialImporter->Import(nullptr, fileInAssets, outputFileName, importSettings);
			break;
		case ResourceType::No_Type_Resource:
			break;
		}
	}
	else
	{
		imported = true;
		App->filesystem->GetFileName(fileInAssets, outputFileName);
	}

	if (imported)
	{
		std::list<Resource*> resources;

		switch (type)
		{
		case ResourceType::Mesh_Resource:
		{
			// Create a new resource for each mesh
			std::list<uint> meshesUUIDs;
			App->GOs->GetMeshResourcesFromScene(outputFileName.data(), meshesUUIDs);

			for (auto it = meshesUUIDs.begin(); it != meshesUUIDs.end(); ++it)
			{
				Resource* resource = CreateNewResource(type, *it);
				resource->file = fileInAssets;
				resource->exportedFileName = outputFileName;
				resources.push_back(resource);
			}

			// If the file has no meta associated, generate a new meta
			if (metaFile == nullptr)
				App->sceneImporter->GenerateMeta(resources, (MeshImportSettings*)importSettings);
		}
		break;
		case ResourceType::Texture_Resource:
		{
			// Create a new resource for the texture
			Resource* resource = CreateNewResource(type);
			resource->file = fileInAssets;
			resource->exportedFileName = outputFileName;
			resources.push_back(resource);

			// If the file has no meta associated, generate a new meta
			if (metaFile == nullptr)
				App->materialImporter->GenerateMeta(resources.front(), (TextureImportSettings*)importSettings);
		}
		break;
		}

		if (resources.size() > 0)
			ret = resources.front()->GetUUID();
	}

	RELEASE(importSettings);

	return ret;
}

ResourceType ModuleResourceManager::GetResourceTypeByExtension(const char* extension)
{
	if (strcmp(extension, ".fbx") == 0 || strcmp(extension, ".FBX") == 0
		|| strcmp(extension, ".obj") == 0 || strcmp(extension, ".OBJ") == 0)
		return ResourceType::Mesh_Resource;
	else if (strcmp(extension, ".dds") == 0 || strcmp(extension, ".DDS") == 0
		|| strcmp(extension, ".png") == 0 || strcmp(extension, ".PNG") == 0
		|| strcmp(extension, ".jpg") == 0 || strcmp(extension, ".JPG") == 0)
		return ResourceType::Texture_Resource;

	return ResourceType::No_Type_Resource;
}

// Get the resource associated to the UUID
const Resource* ModuleResourceManager::GetResource(uint uuid) const
{
	auto it = resources.find(uuid);

	if (it != resources.end())
		return it->second;

	return nullptr;
}

// First argument defines the kind of resource to create. Second argument is used to force and set the uuid.
// In case of uuid set to 0, a random uuid will be generated.
Resource* ModuleResourceManager::CreateNewResource(ResourceType type, uint force_uuid)
{
	assert(type != ResourceType::No_Type_Resource && "Invalid resource type");

	Resource* resource = nullptr;

	uint uuid = force_uuid;
	if (uuid <= 0)
		uuid = App->GenerateRandomNumber();

	switch (type)
	{
	case ResourceType::Mesh_Resource:
		resource = new ResourceMesh(type, uuid);
		break;
	case ResourceType::Texture_Resource:
		resource = new ResourceTexture(type, uuid);
		break;
	}

	if (resource != nullptr)
		resources[uuid] = resource;

	return resource;
}

// Load resource to memory and return number of references. In case of error returns -1.
int ModuleResourceManager::SetAsUsed(uint uuid) const
{
	auto it = resources.find(uuid);

	if (it == resources.end())
		return -1;

	return it->second->LoadToMemory();
}

// Unload resource from memory and return number of references. In case of error returns -1.
int ModuleResourceManager::SetAsUnused(uint uuid) const
{
	auto it = resources.find(uuid);

	if (it == resources.end())
		return -1;

	return it->second->UnloadMemory();
}

// Returns true if resource associated to the uuid can be found and deleted. Returns false in case of error.
bool ModuleResourceManager::DestroyResource(uint uuid)
{
	auto it = resources.find(uuid);

	if (it == resources.end())
		return false;
	
	delete it->second;
	resources.erase(uuid);
	return true;
}

// Deletes all resources.
void ModuleResourceManager::DestroyResources()
{
	for (auto it = resources.begin(); it != resources.end(); ++it)
		delete it->second;

	resources.clear();
}

// Returns true if someone is still referencing to any resource.
bool ModuleResourceManager::SomethingOnMemory() const
{
	for (auto it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->CountReferences() > 0)
			return true;
	}

	return false;
}
