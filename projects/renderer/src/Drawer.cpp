
#include "vulkan/Drawer.hpp"

namespace vulkan
{

Drawer::Drawer( const Device& device,
				const SwapChain& swap_chain,
				const CommandPool& command_pool,
				Shaders shaders )
	: mRenderPass( device, swap_chain ),
	mGraphicsPipeline( device, swap_chain, mRenderPass, shaders ),
	mCommandBuffer( device, mRenderPass, swap_chain, mGraphicsPipeline, command_pool )
{
}

}