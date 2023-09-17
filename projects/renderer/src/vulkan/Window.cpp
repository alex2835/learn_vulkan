#include <vulkan/Window.hpp>

#include <vulkan/Instance.hpp>

#include <exception>
#include <iostream>

using namespace vulkan;


Window::Window( const glm::ivec2& size, const std::string& title )
	: m_size( size ),
	m_title( title ),
	m_surface( VK_NULL_HANDLE ),
	m_framebufferResized( true ),
	m_drawFrameFunc( []( bool& ){} )
{
	// need to init glfw first, to get the suitable glfw extension for the vkinstance
	glfwInit();

	// Disable OpenGL
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow( size.x, size.y, title.c_str(), nullptr, nullptr );
	glfwSetWindowUserPointer( m_window, this );
	glfwSetFramebufferSizeCallback( m_window, FramebufferResizeCallback );
}

Window::~Window()
{
	glfwDestroyWindow( m_window );
	glfwTerminate();
}

void Window::mainLoop()
{
	while( !glfwWindowShouldClose( m_window ) )
	{
		glfwPollEvents();
		m_drawFrameFunc( m_framebufferResized );
	}
}

void Window::CreateSurface( Instance& instance )
{
	if( glfwCreateWindowSurface( instance.handle(), m_window, nullptr, &m_surface ) != VK_SUCCESS )
		throw std::runtime_error( "Unable to create window surface" );
}

void Window::DestroySurface( Instance& instance )
{
	vkDestroySurfaceKHR( instance.handle(), m_surface, nullptr );
}

std::vector<const char*> Window::GetRequiredExtensions()
{
	std::vector<const char*> extensions;
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
	extensions.assign( glfwExtensions, glfwExtensions + glfwExtensionCount );
	return extensions;
}

void Window::FramebufferResizeCallback( GLFWwindow* window, int width, int height )
{
	Window* win = reinterpret_cast<Window*>( glfwGetWindowUserPointer( window ) );
	win->m_framebufferResized = true;
}