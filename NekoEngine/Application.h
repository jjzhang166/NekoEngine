#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleGui.h"

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

public:

	ModuleWindow*		window;
	ModuleInput*		input;
	ModuleAudio*		audio;
	ModuleSceneIntro*	scene_intro;
	ModuleRenderer3D*	renderer3D;
	ModuleCamera3D*		camera;
	ModulePhysics3D*	physics;
	ModuleGui*			gui;

private:

	Timer				ms_timer;
	float				dt;
	std::list<Module*>	list_modules;
};

#endif