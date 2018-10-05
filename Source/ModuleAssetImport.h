#ifndef __MODULE_ASSET_IMPORT_H__
#define __MODULE_ASSET_IMPORT_H__

#include "Module.h"

class ModuleAssetImport : public Module
{
public:

	ModuleAssetImport(bool start_enabled = true);
	~ModuleAssetImport();

	bool Init(JSON_Object* jObject);
	bool CleanUp();

};

#endif