#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__

#include "Panel.h"

#define SCREEN_MIN_WIDTH 640
#define SCREEN_MIN_HEIGHT 480

#define STR_INPUT_SIZE 128

class PanelSettings : public Panel
{
public:

	PanelSettings(char* name);
	virtual ~PanelSettings();

	virtual bool Draw();

private:

	void ApplicationNode() const;
	void WindowNode() const;
	void RendererNode() const;
	void FileSystemNode() const;
	void InputNode() const;
	void HardwareNode() const;
};

#endif
