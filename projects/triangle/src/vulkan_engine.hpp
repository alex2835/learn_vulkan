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

    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;
    VkSurfaceKHR mSurface;

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice mDevice;

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;

    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;
    std::vector<VkImageView> mSwapChainImageViews;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;

    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;

    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    uint32_t mCurrentFrame = 0;

};