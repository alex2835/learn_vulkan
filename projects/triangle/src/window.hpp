#pragma once

#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>
#include <string>

struct WindowSize
{
	int Width = 0;
	int Height = 0;
};

class Window
{
public:
	Window( std::string name, int width, int height );
	~Window();
	void Update();
	bool ShouldClose();
	WindowSize GetSize();
	void WaitEvents();
	void CreateSurface( VkInstance instance, VkSurfaceKHR* surface );


	bool mFramebufferResized = false;
private:
	GLFWwindow* mWindow;
};