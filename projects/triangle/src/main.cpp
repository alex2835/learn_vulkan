

#include "vulkan.hpp"
#include "window.hpp"
#include "vulkan_engine.hpp"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    
    HelloTriangleApplication()
        : mWindow( "Vulkan", WIDTH, HEIGHT ),
          mVulkanEngine( mWindow )
    {}

    void run()
    {
        while( !mWindow.ShouldClose() )
        {
            mWindow.Update(); 
            mVulkanEngine.drawFrame();
        }
        mVulkanEngine.DeviceWaitIdle();
    }

private:
    Window mWindow;
    VulkanEngine mVulkanEngine;
};

int main( void )
{
    try
    {
        HelloTriangleApplication app;
        app.run();
    } 
    catch( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}