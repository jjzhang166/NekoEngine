#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resource.h"

class ResourceTexture : public Resource
{
public:

	ResourceTexture(ResourceType type, uint uuid);
	~ResourceTexture();

private:

	bool LoadInMemory();
	bool UnloadFromMemory();

public:

	uint id = 0;
	uint width = 0;
	uint height = 0;
};

#endif