#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <stdexcept>
#include <vector>

#include "common/non_copyable.hpp"
#include "Window.hpp"

namespace vulkan
{

class Instance : public NonCopyable
{
public:
	Instance( Window& window,
			  const char* appName,
			  const char* engineName,
			  bool validationLayers );
	Instance() = delete;
	~Instance();

	inline const VkInstance& handle() const
	{
		return m_instance;
	}
	inline bool validationLayersEnabled() const
	{
		return m_enableValidationLayers;
	}

	static const std::vector<const char*> ValidationLayers;
	static const std::vector<const char*> DeviceExtensions;

private:
	Window& mWindow;
	VkInstance m_instance;
	bool m_enableValidationLayers;

	static bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions( bool validationLayers );
};

}  // namespace vulkan
