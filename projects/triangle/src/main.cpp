

#include "window.hpp"
#include "vulkan_renderer.hpp"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    
    HelloTriangleApplication()
        : mWindow( "Vulkan", WIDTH, HEIGHT ),
          mVulkanRenderer( mWindow )
    {}

    void run()
    {
        while( !mWindow.ShouldClose() )
        {
            mWindow.Update();
            mVulkanRenderer.drawFrame();
        }
        mVulkanRenderer.DeviceWaitIdle();
    }

private:
    Window mWindow;
    VulkanRenderer mVulkanRenderer;
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