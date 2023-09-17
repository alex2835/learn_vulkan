#pragma once
#include <vulkan/vulkan.h>

#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vulkan/CommandPool.hpp>
#include <vulkan/Device.hpp>
#include <vulkan/ImGui/ImGuiCommandBuffers.hpp>
#include <vulkan/ImGui/ImGuiRenderPass.hpp>
#include <vulkan/Instance.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/Window.hpp>

namespace vulkan
{

class ImGuiApp
{
public:
	ImGuiApp( const Instance& instance,
			  Window& window,
			  const Device& device,
			  const SwapChain& swap_chain,
			  const GraphicsPipeline& graphical_pipeline );
	~ImGuiApp();

	inline VkCommandBuffer& command( uint32_t index )
	{
		return commandBuffers.command( index );
	}
	inline const VkCommandBuffer& command( uint32_t index ) const
	{
		return commandBuffers.command( index );
	}

	void recordCommandBuffers( uint32_t bufferIdx )
	{
		commandBuffers.recordCommandBuffers( bufferIdx );
	}

	void recreate();

private:
	VkDescriptorPool imGuiDescriptorPool;

	//ImGuiRenderPass render_pass;
	RenderPass render_pass;
	CommandPool command_pool;
	ImGuiCommandBuffers commandBuffers;

	const Instance& m_instance;
	const Device& m_device;
	const SwapChain& m_swap_chain;
	//const GraphicsPipeline& m_graphicsPipeline;

	void createImGuiDescriptorPool();
};
}  // namespace vulkan
