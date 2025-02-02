#include "PanelSettings.h"

#ifndef GAMEMODE

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleTimeManager.h"
#include "GameObject.h"
#include "Quadtree.h"

#include "ImGui\imgui.h"
#include "SDL\include\SDL_cpuinfo.h"
#include "SDL\include\SDL_version.h"
#include "mmgr\mmgr.h"
#include <vector>

#include "glew\include\GL\glew.h"

PanelSettings::PanelSettings(char* name) : Panel(name) {}

PanelSettings::~PanelSettings() {}

bool PanelSettings::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		if (ImGui::TreeNode("Application"))
		{
			ApplicationNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Window"))
		{
			if (App->window->IsActive())
				WindowNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Renderer 3D"))
		{
			if (App->renderer3D->IsActive())
				RendererNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("File System"))
		{
			if (App->fs->IsActive())
				FileSystemNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Input"))
		{
			if (App->input->IsActive())
				InputNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Hardware"))
		{
			HardwareNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Scene"))
		{
			if (App->scene->IsActive())
				SceneNode();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Time Manager"))
		{
			if (App->timeManager->IsActive())
				TimeManagerNode();
			ImGui::TreePop();
		}
	}
	ImGui::End();

	return true;
}

void PanelSettings::ApplicationNode() const
{
	// Application name
	static char appName[INPUT_BUF_SIZE];
	if (App->GetAppName() != nullptr)
		strcpy_s(appName, IM_ARRAYSIZE(appName), App->GetAppName());
	if (ImGui::InputText("App Name", appName, IM_ARRAYSIZE(appName)))
		App->SetAppName(appName);

	// Organization name
	static char organizationName[INPUT_BUF_SIZE];
	if (App->GetOrganizationName() != nullptr)
		strcpy_s(organizationName, IM_ARRAYSIZE(organizationName), App->GetOrganizationName());
	if (ImGui::InputText("Organization Name", organizationName, IM_ARRAYSIZE(organizationName)))
		App->SetOrganizationName(organizationName);

	// Cap frames
	bool capFrames = App->GetCapFrames();
	if (ImGui::Checkbox("Cap Frames", &capFrames))
	{
		App->SetCapFrames(capFrames);
		if (capFrames && App->renderer3D->GetVSync())
			App->renderer3D->SetVSync(false);
	}

	if (capFrames)
	{
		int maxFramerate = App->GetMaxFramerate();
		if (ImGui::SliderInt("Max FPS", &maxFramerate, 0, 120))
			App->SetMaxFramerate(maxFramerate);
	}

	// VSync
	bool vsync = App->renderer3D->GetVSync();
	if (ImGui::Checkbox("Use VSync", &vsync))
	{
		App->renderer3D->SetVSync(vsync);
		if (vsync && App->GetCapFrames())
			App->SetCapFrames(false);
	}

	// Framerate
	char title[20];
	std::vector<float> framerateTrack = App->GetFramerateTrack();
	sprintf_s(title, IM_ARRAYSIZE(title), "Framerate %.1f", framerateTrack.back());
	ImGui::PlotHistogram("##framerate", &framerateTrack.front(), framerateTrack.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	// Ms
	std::vector<float> msTrack = App->GetMsTrack();
	sprintf_s(title, IM_ARRAYSIZE(title), "Milliseconds %.1f", msTrack.back());
	ImGui::PlotHistogram("##milliseconds", &msTrack.front(), msTrack.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	sMStats memStats = m_getMemoryStatistics();

	static std::vector<float> memory(100);

	for (uint i = memory.size() - 1; i > 0; --i)
		memory[i] = memory[i - 1];

	memory[0] = (float)memStats.peakActualMemory;

	ImGui::PlotHistogram("##RAMusage", &memory.front(), memory.size(), 0, "Ram Usage", 0.0f, (float)memStats.peakReportedMemory * 1.2f, ImVec2(310, 100));

	ImGui::Text("Total Reported Mem: %u", memStats.totalReportedMemory);
	ImGui::Text("Total Actual Mem: %u", memStats.totalActualMemory);
	ImGui::Text("Peak Reported Mem: %u", memStats.peakReportedMemory);
	ImGui::Text("Peak Actual Mem: %u", memStats.peakActualMemory);
	ImGui::Text("Accumulated Reported Mem: %u", memStats.accumulatedReportedMemory);
	ImGui::Text("Accumulated Actual Mem: %u", memStats.accumulatedActualMemory);
	ImGui::Text("Acumulated Alloc Unit Count: %u", memStats.accumulatedAllocUnitCount);
	ImGui::Text("Total Alloc Unit Count: %u", memStats.totalAllocUnitCount);
	ImGui::Text("Peak Alloc Unit Count: %u", memStats.peakAllocUnitCount);
}

void PanelSettings::WindowNode() const
{
	// Brightness
	float brightness = App->window->GetWindowBrightness();
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		App->window->SetWindowBrightness(brightness);

	// Width, height
	uint screenWidth, screenHeight;
	App->window->GetScreenSize(screenWidth, screenHeight);

	int width = App->window->GetWindowWidth();
	if (ImGui::SliderInt("Width", &width, SCREEN_MIN_WIDTH, screenWidth))
		App->window->SetWindowWidth(width);

	int height = App->window->GetWindowHeight();
	if (ImGui::SliderInt("Height", &height, SCREEN_MIN_HEIGHT, screenHeight))
		App->window->SetWindowHeight(height);

	// Refresh rate
	int refreshRate = App->window->GetRefreshRate();

	ImGui::Text("Refresh rate: ");
	ImGui::SameLine();
	ImGui::TextColored(BLUE, "%i", refreshRate);

	// Fullscreen, resizable, borderless, fullscreen desktop
	static bool fullscreen = App->window->GetFullscreenWindow();
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->SetFullscreenWindow(fullscreen);
	ImGui::SameLine();
	static bool resizable = App->window->GetResizableWindow();
	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->SetResizableWindow(resizable);

	static bool borderless = App->window->GetBorderlessWindow();
	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->SetBorderlessWindow(borderless);
	ImGui::SameLine();
	static bool fullDesktop = App->window->GetFullDesktopWindow();
	if (ImGui::Checkbox("Full Desktop", &fullDesktop))
		App->window->SetFullDesktopWindow(fullDesktop);
}

void PanelSettings::RendererNode() const
{
	GLenum capability = 0;

	capability = GL_DEPTH_TEST;
	bool depthTest = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest))
		App->renderer3D->SetCapabilityState(capability, depthTest);

	capability = GL_CULL_FACE;
	bool cullFace = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_CULL_FACE", &cullFace))
		App->renderer3D->SetCapabilityState(capability, cullFace);

	capability = GL_LIGHTING;
	bool lighting = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_LIGHTING", &lighting))
		App->renderer3D->SetCapabilityState(capability, lighting);

	capability = GL_COLOR_MATERIAL;
	bool colorMaterial = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_COLOR_MATERIAL", &colorMaterial))
		App->renderer3D->SetCapabilityState(capability, colorMaterial);

	capability = GL_TEXTURE_2D;
	bool texture2D = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D))
		App->renderer3D->SetCapabilityState(capability, texture2D);

	capability = GL_LINE_SMOOTH;
	bool lineSmooth = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_LINE_SMOOTH", &lineSmooth))
		App->renderer3D->SetCapabilityState(capability, lineSmooth);

	capability = GL_BLEND;
	bool blend = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_BLEND", &blend))
		App->renderer3D->SetCapabilityState(capability, blend);

	bool wireframeMode = App->renderer3D->IsWireframeMode();
	if (ImGui::Checkbox("Wireframe", &wireframeMode))
		App->renderer3D->SetWireframeMode(wireframeMode);
}

void PanelSettings::FileSystemNode() const
{
	// Paths
	ImGui::Text("Base Path:");
	ImGui::TextColored(BLUE, "%s", App->fs->GetBasePath());
	ImGui::Text("Read Paths:");
	ImGui::TextColored(BLUE, "%s", App->fs->GetReadPaths());
	ImGui::Text("Write Path:");
	ImGui::TextColored(BLUE, "%s", App->fs->GetWritePath());
}

void PanelSettings::InputNode() const
{
	ImGui::Text("Mouse Position:");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%i, %i", App->input->GetMouseX(), App->input->GetMouseY());

	ImGui::Text("Mouse Motion:");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%i, %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());

	ImGui::Text("Mouse Wheel:");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%i", App->input->GetMouseZ());

	ImGui::Separator();
	ImGui::Text("Input LOGS:");

	ImGuiWindowFlags scrollFlags = 0;
	scrollFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

	if (ImGui::BeginChild("scroll", ImVec2(0, 0), false, scrollFlags))
	{
		ImGui::TextUnformatted(buf.begin());
		ImGui::SetScrollHere(1.0f);
	}
	ImGui::EndChild();
}

void PanelSettings::HardwareNode() const
{
	SDL_version version;
	SDL_GetVersion(&version);

	ImGui::Text("SDL version: %i.%i.%i", version.major, version.minor, version.patch);
	ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));

	ImGui::Separator();

	// CPU
	ImGui::Text("CPUs:"); ImGui::SameLine();
	ImGui::TextColored(BLUE, "%i (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

	// RAM
	ImGui::Text("System Ram:"); ImGui::SameLine();
	ImGui::TextColored(BLUE, "%iMb", SDL_GetSystemRAM());

	// Capabilites
	ImGui::Text("Caps:"); ImGui::SameLine();
	ImGui::TextColored(BLUE, "%s%s%s%s%s%s%s%s%s%s%s", (SDL_HasAVX()) ? "AVX " : "", (SDL_HasAVX2()) ? "AVX2 " : "", (SDL_HasAltiVec()) ? "AltiVec " : "",
		(SDL_Has3DNow()) ? "3DNow " : "", (SDL_HasMMX()) ? "MMX " : "", (SDL_HasRDTSC()) ? "RDTSC " : "", (SDL_HasSSE()) ? "SEE " : "",
		(SDL_HasSSE2()) ? "SSE2 " : "", (SDL_HasSSE3()) ? "SSE3 " : "", (SDL_HasSSE41()) ? "SSE41 " : "",
		(SDL_HasSSE42()) ? "SSE42 " : "");

	ImGui::Separator();

	ImGui::Text("GPU:"); ImGui::SameLine();
	ImGui::TextColored(BLUE, "%s", glGetString(GL_RENDERER));
	ImGui::Text("Brand:"); ImGui::SameLine();
	ImGui::TextColored(BLUE, "%s", glGetString(GL_VENDOR));
}

void PanelSettings::SceneNode() const
{
	ImGui::Text("Quadtree");
	if (ImGui::Button("Create Random Game Object")) { App->scene->CreateRandomStaticGameObject(); }

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::TreeNodeEx("Root Node (Subdivision 0)"))
	{
		QuadtreeNode* root = App->scene->quadtree.root;

		for (std::list<GameObject*>::const_iterator it = root->objects.begin(); it != root->objects.end(); ++it)
			ImGui::TextColored(BLUE, (*it)->GetName());

		RecursiveDrawQuadtreeHierarchy(root);
		ImGui::TreePop();
	}
}

void PanelSettings::TimeManagerNode() const
{
	ImGui::Text("Game Clock");
	ImGui::Spacing();

	ImGui::Text("Game time scale:");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%f", App->timeManager->GetTimeScale());

	ImGui::Text("Time since game start (seconds):");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%f", App->timeManager->GetTime());

	ImGui::Text("Dt (seconds):");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%f", App->timeManager->GetDt());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Real Time Clock");
	ImGui::Spacing();

	ImGui::Text("Frame count:");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%i", App->timeManager->GetFrameCount());

	ImGui::Text("Real time since game start (seconds):");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%f", App->timeManager->GetRealTime());

	ImGui::Text("Real dt (seconds):");
	ImGui::SameLine(); ImGui::TextColored(BLUE, "%f", App->timeManager->GetRealDt());
}

void PanelSettings::AddInput(const char* input)
{
	int oldSize = buf.size();

	buf.appendf(input);

	for (int newSize = buf.size(); oldSize < newSize; ++oldSize)
	{
		if (buf[oldSize] == '\n')
			lineOffsets.push_back(oldSize);
	}
}

void PanelSettings::RecursiveDrawQuadtreeHierarchy(QuadtreeNode* node) const
{
	ImGuiTreeNodeFlags treeNodeFlags = 0;
	treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

	if (!node->IsLeaf())
	{
		for (uint i = 0; i < 4; ++i)
		{
			QuadtreeNode* child = node->children[i];

			if (child->objects.size() > 0 || !child->IsLeaf())
			{
				bool treeNodeOpened = false;

				treeNodeFlags = 0;
				treeNodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

				char name[DEFAULT_BUF_SIZE];
				if (!child->IsLeaf())
					sprintf_s(name, DEFAULT_BUF_SIZE, "Node %i (Subdivison %i)", i, child->subdivision);
				else
					sprintf_s(name, DEFAULT_BUF_SIZE, "Leaf Node %i (Subdivison %i)", i, child->subdivision);
				if (ImGui::TreeNodeEx(name, treeNodeFlags))
					treeNodeOpened = true;

				if (treeNodeOpened)
				{
					for (std::list<GameObject*>::const_iterator it = child->objects.begin(); it != child->objects.end(); ++it)
						ImGui::TextColored(BLUE, (*it)->GetName());

					if (!child->IsLeaf())
						RecursiveDrawQuadtreeHierarchy(child);

					ImGui::TreePop();
				}
			}
			else
			{
				treeNodeFlags = 0;
				treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

				char name[DEFAULT_BUF_SIZE];
				sprintf_s(name, DEFAULT_BUF_SIZE, "Leaf Node %i (Subdivison %i)", i, child->subdivision);
				ImGui::TreeNodeEx(name, treeNodeFlags);

				ImGui::TreePop();
			}
		}
	}
}

#endif // GAME