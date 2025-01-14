#ifndef __PANEL_ASSETS_H__
#define __PANEL_ASSETS_H__

#include "GameMode.h"

#ifndef GAMEMODE

#include "Panel.h"

#include "ResourceTypes.h"
#include "Globals.h"
#include "ResourceShaderObject.h"

struct AssetsFile;
struct Resource;

class PanelAssets : public Panel
{
public:

	PanelAssets(char* name);
	~PanelAssets();

	bool Draw();

private:

	void RecursiveDrawAssetsDir(AssetsFile* assetsFile);
	void SetResourceDragAndDropSource(ResourceType type, uint UUID = 0, const Resource* resource = nullptr, const char* file = nullptr) const;
	
	void CreateShaderPopUp(const char* path);
	void DeleteShaderPopUp(const char* path);
	void CreateShaderConfirmationPopUp();
	void DeleteShaderConfirmationPopUp();

public:

	ShaderType shaderType = ShaderType::NoShaderType;
	std::string shaderFile;
	char shaderName[INPUT_BUF_SIZE];

	bool showCreateShaderConfirmationPopUp = false;
	bool showDeleteShaderConfirmationPopUp = false;
};

#endif

#endif // GAME
