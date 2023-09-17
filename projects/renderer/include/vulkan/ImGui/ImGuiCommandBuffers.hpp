#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <vulkan/CommandBuffers.hpp>

namespace vulkan
{

class ImGuiCommandBuffers : public CommandBuffers
{
public:
	ImGuiCommandBuffers( const Device& device,
						 const RenderPass& renderpass,
						 const SwapChain& swap_chain,
						 const GraphicsPipeline& graphical_pipeline,
						 const CommandPool& command_pool );

	void recordCommandBuffers( uint32_t bufferIdx );
	void recreate();

private:
	void createCommandBuffers();
};

}  // namespace vulkan
