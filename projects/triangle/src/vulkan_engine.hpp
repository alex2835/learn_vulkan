#pragma once

#include "window.hpp"
#include <vector>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>


inline std::vector<char> readFile( const std::string& filename )
{
    std::ifstream file( filename, std::ios::ate | std::ios::binary );

    if( !file.is_open() )
        throw std::runtime_error( "failed to open file!" );

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer( fileSize );

    file.seekg( 0 );
    file.read( buffer.data(), fileSize );

    file.close();

    return buffer;
}





struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};





class VulkanEngine
{
public:
    VulkanEngine();
    VulkanEngine( Window& window );

    void initVulkan();
    void cleanupSwapChain();
    void cleanup();
    void recreateSwapChain();
    void createInstance();
    void populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& createInfo );
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex );
    void createSyncObjects();
    void drawFrame();
    VkShaderModule createShaderModule( const std::vector<char>& code );
    VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats );
    VkPresentModeKHR chooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes );
    VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );
    SwapChainSupportDetails querySwapChainSupport( VkPhysicalDevice device );
    bool isDeviceSuitable( VkPhysicalDevice device );
    bool checkDeviceExtensionSupport( VkPhysicalDevice device );
    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device );
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    void DeviceWaitIdle();


private:

    Window& mWindow;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

};