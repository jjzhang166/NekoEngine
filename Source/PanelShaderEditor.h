#ifndef __PANEL_SHADER_EDITOR_H__
#define __PANEL_SHADER_EDITOR_H__

#include "Panel.h"

#ifndef GAMEMODE

#include <list>
#include "Globals.h"

class ResourceShaderObject;
class ResourceShaderProgram;

class PanelShaderEditor : public Panel
{
public:

	PanelShaderEditor(char* name);
	~PanelShaderEditor();

	bool Draw();
	
	void OpenShaderInShaderEditor(ResourceShaderProgram* program);

private:

	std::list<ResourceShaderObject*> GetShaderObjects() const;

private:

	ResourceShaderProgram* shaderProgram = nullptr;

public:

	char shaderProgramName[INPUT_BUF_SIZE];

	std::list<ResourceShaderObject*> vertexShaders;
	std::list<ResourceShaderObject*> fragmentShaders;
};

#endif

#endif
