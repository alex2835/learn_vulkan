#pragma once

#include "vulkan/RenderPass.hpp"
#include "vulkan/GraphicsPipeline.hpp"
#include "vulkan/CommandBuffers.hpp"
#include "vulkan/CommandPool.hpp"
#include "vulkan/Shader.hpp"

namespace vulkan
{

class Drawer
{
public:
	Drawer( const Device& device, 
			const SwapChain& swap_chain,
			const CommandPool& command_pool,
			Shaders shaders );


private:
	RenderPass mRenderPass;
	GraphicsPipeline mGraphicsPipeline;
	CommandBuffers mCommandBuffer;
};


}