
#include "vulkan_engine.hpp"

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT( VkInstance mInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger )
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( mInstance, "vkCreateDebugUtilsMessengerEXT" );
    if( func != nullptr )
        return func( mInstance, pCreateInfo, pAllocator, pDebugMessenger );
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT( VkInstance mInstance, VkDebugUtilsMessengerEXT mDebugMessenger, const VkAllocationCallbacks* pAllocator )
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( mInstance, "vkDestroyDebugUtilsMessengerEXT" );
    if( func != nullptr )
        func( mInstance, mDebugMessenger, pAllocator );
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                              void* pUserData )
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}






VulkanEngine::VulkanEngine( Window& window )
	: mWindow( window )
{
    initVulkan();
}

void VulkanEngine::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void VulkanEngine::cleanupSwapChain()
{
    for( auto framebuffer : mSwapChainFramebuffers )
        vkDestroyFramebuffer( mDevice, framebuffer, nullptr );

    for( auto imageView : mSwapChainImageViews )
        vkDestroyImageView( mDevice, imageView, nullptr );

    vkDestroySwapchainKHR( mDevice, mSwapChain, nullptr );
}

void VulkanEngine::cleanup()
{
    cleanupSwapChain();

    vkDestroyPipeline( mDevice, mGraphicsPipeline, nullptr );
    vkDestroyPipelineLayout( mDevice, mPipelineLayout, nullptr );
    vkDestroyRenderPass( mDevice, mRenderPass, nullptr );

    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        vkDestroySemaphore( mDevice, mRenderFinishedSemaphores[i], nullptr );
        vkDestroySemaphore( mDevice, mImageAvailableSemaphores[i], nullptr );
        vkDestroyFence( mDevice, mInFlightFences[i], nullptr );
    }

    vkDestroyCommandPool( mDevice, mCommandPool, nullptr );
    vkDestroyDevice( mDevice, nullptr );

    if( enableValidationLayers )
        DestroyDebugUtilsMessengerEXT( mInstance, mDebugMessenger, nullptr );

    vkDestroySurfaceKHR( mInstance, mSurface, nullptr );
    vkDestroyInstance( mInstance, nullptr );

}

void VulkanEngine::recreateSwapChain()
{
    WindowSize window_size = mWindow.GetSize();
    while( window_size.Width == 0 || window_size.Height == 0 )
    {
        mWindow.WaitEvents();
        window_size = mWindow.GetSize();
    }
    vkDeviceWaitIdle( mDevice );

    cleanupSwapChain();
    
    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void VulkanEngine::createInstance()
{
    if( enableValidationLayers && !checkValidationLayerSupport() )
        throw std::runtime_error( "validation layers requested, but not available!" );

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if( enableValidationLayers )
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo( debugCreateInfo );
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }
    
    if( vkCreateInstance( &createInfo, nullptr, &mInstance ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create mInstance!" );
}

void VulkanEngine::populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& createInfo )
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanEngine::setupDebugMessenger()
{
    if( !enableValidationLayers )
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo( createInfo );

    if( CreateDebugUtilsMessengerEXT( mInstance, &createInfo, nullptr, &mDebugMessenger ) != VK_SUCCESS )
        throw std::runtime_error( "failed to set up debug messenger!" );
}

void VulkanEngine::createSurface()
{
    mWindow.CreateSurface( mInstance, &mSurface );
}

void VulkanEngine::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( mInstance, &deviceCount, nullptr );

    if( deviceCount == 0 )
        throw std::runtime_error( "failed to find GPUs with Vulkan support!" );

    std::vector<VkPhysicalDevice> devices( deviceCount );
    vkEnumeratePhysicalDevices( mInstance, &deviceCount, devices.data() );

    for( const auto& device : devices )
    {
        if( isDeviceSuitable( device ) )
        {
            mPhysicalDevice = device;
            break;
        }
    }

    if( mPhysicalDevice == VK_NULL_HANDLE )
        throw std::runtime_error( "failed to find a suitable GPU!" );
}

void VulkanEngine::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies( mPhysicalDevice );

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for( uint32_t queueFamily : uniqueQueueFamilies )
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back( queueCreateInfo );
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>( deviceExtensions.size() );
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if( enableValidationLayers )
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    if( vkCreateDevice( mPhysicalDevice, &createInfo, nullptr, &mDevice ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create logical mDevice!" );

    vkGetDeviceQueue( mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue );
    vkGetDeviceQueue( mDevice, indices.presentFamily.value(), 0, &mPresentQueue );
}

void VulkanEngine::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport( mPhysicalDevice );

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapChainSupport.formats );
    VkPresentModeKHR presentMode = chooseSwapPresentMode( swapChainSupport.presentModes );
    VkExtent2D extent = chooseSwapExtent( swapChainSupport.capabilities );

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if( swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount )
        imageCount = swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies( mPhysicalDevice );
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if( indices.graphicsFamily != indices.presentFamily )
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if( vkCreateSwapchainKHR( mDevice, &createInfo, nullptr, &mSwapChain ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create swap chain!" );

    vkGetSwapchainImagesKHR( mDevice, mSwapChain, &imageCount, nullptr );
    mSwapChainImages.resize( imageCount );
    vkGetSwapchainImagesKHR( mDevice, mSwapChain, &imageCount, mSwapChainImages.data() );

    mSwapChainImageFormat = surfaceFormat.format;
    mSwapChainExtent = extent;
}

void VulkanEngine::createImageViews()
{
    mSwapChainImageViews.resize( mSwapChainImages.size() );

    for( size_t i = 0; i < mSwapChainImages.size(); i++ )
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = mSwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if( vkCreateImageView( mDevice, &createInfo, nullptr, &mSwapChainImageViews[i] ) != VK_SUCCESS )
            throw std::runtime_error( "failed to create image views!" );
    }
}

void VulkanEngine::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if( vkCreateRenderPass( mDevice, &renderPassInfo, nullptr, &mRenderPass ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create render pass!" );
}

void VulkanEngine::createGraphicsPipeline()
{
    auto vertShaderCode = readFile( "shaders/vert.spv" );
    auto fragShaderCode = readFile( "shaders/frag.spv" );

    VkShaderModule vertShaderModule = createShaderModule( vertShaderCode );
    VkShaderModule fragShaderModule = createShaderModule( fragShaderCode );

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if( vkCreatePipelineLayout( mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create pipeline layout!" );

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if( vkCreateGraphicsPipelines( mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create graphics pipeline!" );

    vkDestroyShaderModule( mDevice, fragShaderModule, nullptr );
    vkDestroyShaderModule( mDevice, vertShaderModule, nullptr );
}

void VulkanEngine::createFramebuffers()
{
    mSwapChainFramebuffers.resize( mSwapChainImageViews.size() );

    for( size_t i = 0; i < mSwapChainImageViews.size(); i++ )
    {
        VkImageView attachments[] = {
            mSwapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = mSwapChainExtent.width;
        framebufferInfo.height = mSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if( vkCreateFramebuffer( mDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i] ) != VK_SUCCESS )
            throw std::runtime_error( "failed to create framebuffer!" );
    }
}

void VulkanEngine::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies( mPhysicalDevice );

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if( vkCreateCommandPool( mDevice, &poolInfo, nullptr, &mCommandPool ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create command pool!" );
}

void VulkanEngine::createCommandBuffers()
{
    mCommandBuffers.resize( MAX_FRAMES_IN_FLIGHT );

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();

    if( vkAllocateCommandBuffers( mDevice, &allocInfo, mCommandBuffers.data() ) != VK_SUCCESS )
        throw std::runtime_error( "failed to allocate command buffers!" );
}

void VulkanEngine::recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex )
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS )
        throw std::runtime_error( "failed to begin recording command buffer!" );

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = mSwapChainExtent;

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline );

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)mSwapChainExtent.width;
    viewport.height = (float)mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = mSwapChainExtent;
    vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

    vkCmdDraw( commandBuffer, 3, 1, 0, 0 );

    vkCmdEndRenderPass( commandBuffer );

    if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
        throw std::runtime_error( "failed to record command buffer!" );
}

void VulkanEngine::createSyncObjects()
{
    mImageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
    mRenderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
    mInFlightFences.resize( MAX_FRAMES_IN_FLIGHT );

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        if( vkCreateSemaphore( mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i] ) != VK_SUCCESS ||
            vkCreateSemaphore( mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i] ) != VK_SUCCESS ||
            vkCreateFence( mDevice, &fenceInfo, nullptr, &mInFlightFences[i] ) != VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create synchronization objects for a frame!" );
        }
    }
}

void VulkanEngine::drawFrame()
{
    vkWaitForFences( mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX );

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR( mDevice, mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex );

    if( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        recreateSwapChain();
        return;
    }
    else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
        throw std::runtime_error( "failed to acquire swap chain image!" );

    vkResetFences( mDevice, 1, &mInFlightFences[mCurrentFrame] );

    vkResetCommandBuffer( mCommandBuffers[mCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0 );
    recordCommandBuffer( mCommandBuffers[mCurrentFrame], imageIndex );

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

    VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if( vkQueueSubmit( mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame] ) != VK_SUCCESS )
        throw std::runtime_error( "failed to submit draw command buffer!" );

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { mSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR( mPresentQueue, &presentInfo );

    if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mWindow.mFramebufferResized )
        recreateSwapChain();

    else if( result != VK_SUCCESS )
        throw std::runtime_error( "failed to present swap chain image!" );

    mCurrentFrame = ( mCurrentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
}

VkShaderModule VulkanEngine::createShaderModule( const std::vector<char>& code )
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>( code.data() );

    VkShaderModule shaderModule;
    if( vkCreateShaderModule( mDevice, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
        throw std::runtime_error( "failed to create shader module!" );

    return shaderModule;
}

VkSurfaceFormatKHR VulkanEngine::chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats )
{
    for( const auto& availableFormat : availableFormats )
        if( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
            return availableFormat;

    return availableFormats[0];
}

VkPresentModeKHR VulkanEngine::chooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes )
{
    for( const auto& availablePresentMode : availablePresentModes )
        if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
            return availablePresentMode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanEngine::chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities )
{
    if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
        return capabilities.currentExtent;
    else
    {
        auto [width, height] = mWindow.GetSize();

        VkExtent2D actualExtent = {
            static_cast<uint32_t>( width ),
            static_cast<uint32_t>( height )
        };

        actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );

        return actualExtent;
    }
}

SwapChainSupportDetails VulkanEngine::querySwapChainSupport( VkPhysicalDevice mDevice )
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( mDevice, mSurface, &details.capabilities );

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR( mDevice, mSurface, &formatCount, nullptr );

    if( formatCount != 0 )
    {
        details.formats.resize( formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( mDevice, mSurface, &formatCount, details.formats.data() );
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR( mDevice, mSurface, &presentModeCount, nullptr );

    if( presentModeCount != 0 )
    {
        details.presentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( mDevice, mSurface, &presentModeCount, details.presentModes.data() );
    }

    return details;
}

bool VulkanEngine::isDeviceSuitable( VkPhysicalDevice mDevice )
{
    QueueFamilyIndices indices = findQueueFamilies( mDevice );

    bool extensionsSupported = checkDeviceExtensionSupport( mDevice );

    bool swapChainAdequate = false;
    if( extensionsSupported )
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport( mDevice );
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool VulkanEngine::checkDeviceExtensionSupport( VkPhysicalDevice mDevice )
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties( mDevice, nullptr, &extensionCount, nullptr );

    std::vector<VkExtensionProperties> availableExtensions( extensionCount );
    vkEnumerateDeviceExtensionProperties( mDevice, nullptr, &extensionCount, availableExtensions.data() );

    std::set<std::string> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end() );

    for( const auto& extension : availableExtensions )
        requiredExtensions.erase( extension.extensionName );

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanEngine::findQueueFamilies( VkPhysicalDevice mDevice )
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( mDevice, &queueFamilyCount, nullptr );

    std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties( mDevice, &queueFamilyCount, queueFamilies.data() );

    int i = 0;
    for( const auto& queueFamily : queueFamilies )
    {

        if( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( mDevice, i, mSurface, &presentSupport );

        if( presentSupport )
            indices.presentFamily = i;

        if( indices.isComplete() )
            break;

        i++;
    }

    return indices;
}

std::vector<const char*> VulkanEngine::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

    std::vector<const char*> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

    if( enableValidationLayers )
        extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );

    return extensions;
}

bool VulkanEngine::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

    std::vector<VkLayerProperties> availableLayers( layerCount );
    vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

    for( const char* layerName : validationLayers )
    {
        bool layerFound = false;

        for( const auto& layerProperties : availableLayers )
        {
            if( strcmp( layerName, layerProperties.layerName ) == 0 )
            {
                layerFound = true;
                break;
            }
        }
        if( !layerFound )
            return false;
    }

    return true;
}

void VulkanEngine::DeviceWaitIdle()
{
    vkDeviceWaitIdle( mDevice );
}
