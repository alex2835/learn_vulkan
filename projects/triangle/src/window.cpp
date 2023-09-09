
#include "window.hpp"
#include <stdexcept>

void FramebufferResizeCallback( GLFWwindow* window, int width, int height )
{
    auto w = reinterpret_cast<Window*>( glfwGetWindowUserPointer( window ) );
    w->mFramebufferResized = true;
}

Window::Window( std::string name, int width, int height )
{
    glfwInit();
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

    mWindow = glfwCreateWindow( width, height, name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer( mWindow, this );
    glfwSetFramebufferSizeCallback( mWindow, FramebufferResizeCallback );
}

Window::~Window()
{
    glfwDestroyWindow( mWindow );
    glfwTerminate();
}

void Window::Update()
{
    mFramebufferResized = false;
    glfwPollEvents();
}

WindowSize Window::GetSize()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize( mWindow, &width, &height );
    return { width, height };
}

void Window::WaitEvents()
{
    mFramebufferResized = false;
    glfwWaitEvents();
}

void Window::CreateSurface( VkInstance instance, VkSurfaceKHR* surface )
{
    if( glfwCreateWindowSurface( instance, mWindow, nullptr, surface ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create window surface!" );
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose( mWindow );
}
