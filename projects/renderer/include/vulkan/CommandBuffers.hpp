#pragma once
#include <vulkan/vulkan.h>
#include "common/non_copyable.hpp"
#include <functional>
#include <vector>
#include <vulkan/CommandPool.hpp>
#include <vulkan/Device.hpp>
#include <vulkan/GraphicsPipeline.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/SwapChain.hpp>

namespace vulkan
{

class CommandBuffers : public NonCopyable
{
public:
	CommandBuffers( const Device& device,
					const RenderPass& renderpass,
					const SwapChain& swap_chain,
					const GraphicsPipeline& graphical_pipeline,
					const CommandPool& command_pool );
	~CommandBuffers();

	void createCommandBuffers();
	void recreate();

	inline VkCommandBuffer& command( uint32_t index )
	{
		return m_commandBuffers[index];
	}
	inline const VkCommandBuffer& command( uint32_t index ) const
	{
		return m_commandBuffers[index];
	}

	static void SingleTimeCommands( const Device& device,
									const CommandPool& cmdPool,
									const std::function<void( const VkCommandBuffer& )>& func );

protected:
	std::vector<VkCommandBuffer> m_commandBuffers;

	const Device& m_device;
	const RenderPass& m_render_pass;
	const SwapChain& m_swap_chain;
	const CommandPool& m_command_pool;
	const GraphicsPipeline& m_graphicsPipeline;

	//virtual void createCommandBuffers() = 0;
	void destroyCommandBuffers();
};
}  // namespace vulkan
