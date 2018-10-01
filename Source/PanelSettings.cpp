#include "PanelSettings.h"

#include "Application.h"
#include "ModuleRenderer3D.h"

#include "ImGui/imgui.h"
#include "SDL/include/SDL_cpuinfo.h"
#include "SDL/include/SDL_version.h"
#include "mmgr/mmgr.h"
#include <vector>

#include <windows.h>
#include "glew\include\GL\glew.h"

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX			0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX		0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX    0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX				0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX				0x904B

PanelSettings::PanelSettings(char* name) : Panel(name)
{
}

PanelSettings::~PanelSettings()
{
}

bool PanelSettings::Draw()
{
	ImGui::Begin(name, &enabled);
	if (ImGui::TreeNode("Application"))
	{
		ApplicationNode();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Window"))
	{
		WindowNode();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Renderer"))
	{
		RendererNode();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("File System"))
	{
		FileSystemNode();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Input"))
	{
		InputNode();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Hardware"))
	{
		HardwareNode();
		ImGui::TreePop();
	}
#if _DEBUG
	if (ImGui::TreeNode("Demo Window"))
	{
		ImGui::ShowDemoWindow();
		ImGui::TreePop();
	}
#endif
	ImGui::End();

	return true;
}

void PanelSettings::ApplicationNode() const
{
	// Application name
	static char appName[STR_INPUT_SIZE];
	if (App->GetAppName() != nullptr)
		strcpy_s(appName, IM_ARRAYSIZE(appName), App->GetAppName());
	if (ImGui::InputText("App Name", appName, IM_ARRAYSIZE(appName)))
		App->SetAppName(appName);

	// Organization name
	static char organizationName[STR_INPUT_SIZE];
	if (App->GetOrganizationName() != nullptr)
		strcpy_s(organizationName, IM_ARRAYSIZE(organizationName), App->GetOrganizationName());
	if (ImGui::InputText("Organization Name", organizationName, IM_ARRAYSIZE(organizationName)))
		App->SetOrganizationName(organizationName);

	// Cap frames
	static bool capFrames = App->GetCapFrames();
	if (ImGui::Checkbox("Cap Frames", &capFrames))
		App->SetCapFrames(capFrames);

	if (capFrames)
	{
		int maxFramerate = App->GetMaxFramerate();
		if (ImGui::SliderInt("Max FPS", &maxFramerate, 0, 120))
			App->SetMaxFramerate(maxFramerate);
	}

	// VSync
	static bool vsync = App->renderer3D->GetVSync();
	if (ImGui::Checkbox("Use VSync", &vsync))
		App->renderer3D->SetVSync(vsync);

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
	// Active
	static bool active = App->window->GetWindowActive();
	if (ImGui::Checkbox("Active", &active))
		App->window->SetWindowActive(active);

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
	ImGui::TextColored({ 239, 201, 0, 255 }, "%i", refreshRate);

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
	static bool depthTest = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_DEPTH_TEST", &depthTest))
		App->renderer3D->SetCapabilityState(capability, depthTest);

	capability = GL_CULL_FACE;
	static bool cullFace = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_CULL_FACE", &cullFace))
		App->renderer3D->SetCapabilityState(capability, cullFace);

	capability = GL_LIGHTING;
	static bool lighting = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_LIGHTING", &lighting))
		App->renderer3D->SetCapabilityState(capability, lighting);

	capability = GL_COLOR_MATERIAL;
	static bool colorMaterial = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_COLOR_MATERIAL", &colorMaterial))
		App->renderer3D->SetCapabilityState(capability, colorMaterial);

	capability = GL_TEXTURE_2D;
	static bool texture2D = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_TEXTURE_2D", &texture2D))
		App->renderer3D->SetCapabilityState(capability, texture2D);

	capability = GL_LINE_SMOOTH;
	static bool lineSmooth = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_LINE_SMOOTH", &lineSmooth))
		App->renderer3D->SetCapabilityState(capability, lineSmooth);

	capability = GL_FOG;
	static bool fog = App->renderer3D->GetCapabilityState(capability);
	if (ImGui::Checkbox("GL_FOG", &fog))
		App->renderer3D->SetCapabilityState(capability, fog);

	static bool wireframeMode = App->renderer3D->IsWireframeMode();
	if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
		App->renderer3D->SetWireframeMode(wireframeMode);
}

void PanelSettings::FileSystemNode() const
{

}

void PanelSettings::InputNode() const
{

}

void PanelSettings::HardwareNode() const
{
	SDL_version version;
	SDL_GetVersion(&version);

	ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));
	ImGui::Text("SDL version: %i.%i.%i", version.major, version.minor, version.patch);

	ImGui::Separator();

	// CPU
	ImGui::Text("CPUs:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%i (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

	// RAM
	ImGui::Text("System Ram:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%iMb", SDL_GetSystemRAM());

	// Capabilites
	ImGui::Text("Caps:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%s%s%s%s%s%s%s%s%s%s%s", (SDL_HasAVX()) ? "AVX " : "", (SDL_HasAVX2()) ? "AVX2 " : "", (SDL_HasAltiVec()) ? "AltiVec " : "",
		(SDL_Has3DNow()) ? "3DNow " : "", (SDL_HasMMX()) ? "MMX " : "", (SDL_HasRDTSC()) ? "RDTSC " : "", (SDL_HasSSE()) ? "SEE " : "",
		(SDL_HasSSE2()) ? "SSE2 " : "", (SDL_HasSSE3()) ? "SSE3 " : "", (SDL_HasSSE41()) ? "SSE41 " : "",
		(SDL_HasSSE42()) ? "SSE42 " : "");

	ImGui::Separator();

	//TODO: fix issue if pc has 2 or more gpus
	//Gpu
	ImGui::Text("Gpu:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%s", glGetString(GL_RENDERER));
	ImGui::Text("Brand:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%s", glGetString(GL_VENDOR));

	GLint totalMemory;
	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemory);
	GLint memoryUsage = 0;
	GLint reservedMemory = 0;
	GLint availableMemory = 0;
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableMemory);
	glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &reservedMemory);
	memoryUsage = totalMemory - availableMemory;

	ImGui::Text("VRAM Budget:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%.2fMb", totalMemory * 0.001);
	ImGui::Text("VRAM Usage:"); ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%.2fMb", memoryUsage * 0.001);
	ImGui::Text("VRAM Available:");	ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%.2fMb", availableMemory * 0.001);
	ImGui::Text("VRAM Reserved:");	ImGui::SameLine();
	ImGui::TextColored({ 239,201,0,255 }, "%.2fMb", reservedMemory * 0.001);
}