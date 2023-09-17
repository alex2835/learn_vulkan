#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

//#include <vulkan/Triangle/TriangleCommandBuffers.hpp>
#include <vulkan/DebugUtilsMessenger.hpp>
#include <vulkan/Device.hpp>
#include <vulkan/GraphicsPipeline.hpp>
#include <vulkan/ImGui/ImGuiApp.hpp>
#include <vulkan/Instance.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/SyncObjects.hpp>
#include <vulkan/Window.hpp>

namespace vulkan
{

class Application
{
public:
	Application();

	void run()
	{
		mainLoop();
	}

private:
	Window window;
	Instance instance;
	DebugUtilsMessenger debugMessenger;
	Device device;
	SwapChain swap_chain;
	CommandPool command_pool;
	SyncObjects syncObjects;
	
	RenderPass render_pass;
	GraphicsPipeline graphicsPipeline;
	CommandBuffers commandBuffers;
	ImGuiApp interface;

	size_t currentFrame = 0;

	void mainLoop();

	void drawFrame( bool& framebufferResized );
	void drawImGui();

	void recreateSwapChain( bool& framebufferResized );
};

}  // namespace vulkan