#include <vulkan/Instance.hpp>

#include <vulkan/DebugUtilsMessenger.hpp>
#include <vulkan/Window.hpp>

using namespace vulkan;

const std::vector<const char*> Instance::ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> Instance::DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

Instance::Instance( Window& window,
					const char* appName,
					const char* engineName,
					bool validationLayers )
	: mWindow( window ),
	m_instance( VK_NULL_HANDLE ),
	m_enableValidationLayers( validationLayers )
{
	if( validationLayers && !CheckValidationLayerSupport() )
		throw std::runtime_error( "validation layers requested, but not available!" );

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
	appInfo.pEngineName = engineName;
	appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions( validationLayers );
	createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if( validationLayers )
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>( Instance::ValidationLayers.size() );
		createInfo.ppEnabledLayerNames = Instance::ValidationLayers.data();
		DebugUtilsMessenger::PopulateDebugMessengerCreateInfo( debugCreateInfo );
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}
	if( vkCreateInstance( &createInfo, nullptr, &m_instance ) != VK_SUCCESS )
		throw std::runtime_error( "failed to create instance!" );

	mWindow.CreateSurface( *this );
}

Instance::~Instance()
{
	mWindow.DestroySurface( *this );
	vkDestroyInstance( m_instance, nullptr );
}

bool Instance::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	for( const char* layerName : Instance::ValidationLayers )
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
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> Instance::GetRequiredExtensions( bool validationLayers )
{
	auto extensions = mWindow.GetRequiredExtensions();
	if( validationLayers )
		extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	return extensions;
}