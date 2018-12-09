#ifndef __RESOURCE_SHADER_OBJECT_H__
#define __RESOURCE_SHADER_OBJECT_H__

#include "Resource.h"

#include "glew\include\GL\glew.h"

enum ShaderType
{
	NoShaderType,
	VertexShaderType,
	FragmentShaderType
};

class ResourceShaderObject : public Resource
{
public:

	ResourceShaderObject(ResourceType type, uint uuid);
	~ResourceShaderObject();

	uint LoadMemory();
	uint UnloadMemory() { return 0; }

	void SetSource(const char* source, uint size);
	const char* GetSource() const;

	bool Compile();

	bool IsObjectCompiled() const;

	static ShaderType GetShaderTypeByExtension(const char* extension);

private:

	bool LoadInMemory();
	bool UnloadFromMemory() { return true; }

private:

	char* source = nullptr;

public:

	ShaderType shaderType = ShaderType::NoShaderType;
	GLuint shaderObject = 0;
};

#endif // __RESOURCE_SHADER_OBJECT_H__