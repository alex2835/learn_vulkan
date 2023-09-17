#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "common/non_copyable.hpp"

namespace vulkan
{

class Instance;

class Window : public NonCopyable
{
public:
	// using DrawFrameFunc = void(*)(bool& framebufferResized);

	Window( const glm::ivec2& size, const std::string& title );
	Window() = delete;
	~Window();
	void mainLoop();

	inline const glm::ivec2& size() const
	{
		return m_size;
	}

	inline const GLFWwindow* window() const
	{
		return m_window;
	}
	inline GLFWwindow* window()
	{
		return m_window;
	}

	inline const VkSurfaceKHR& surface() const
	{
		return m_surface;
	}

	inline void framebufferSize( glm::ivec2& size ) const
	{
		glfwGetFramebufferSize( m_window, &size[0], &size[1] );
	}

	inline void setDrawFrameFunc( const std::function<void( bool& )>& func )
	{
		m_drawFrameFunc = func;
	}

	void CreateSurface( Instance& instance );
	void DestroySurface( Instance& instance );

	std::vector<const char*> GetRequiredExtensions();

private:
	GLFWwindow* m_window;
	glm::ivec2 m_size;
	std::string m_title;
	VkSurfaceKHR m_surface;
	bool m_framebufferResized;
	std::function<void( bool& )> m_drawFrameFunc;

	static void FramebufferResizeCallback( GLFWwindow* window, int width, int height );
};

}  // namespace vulkan
