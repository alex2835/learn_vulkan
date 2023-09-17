#include <vulkan/CommandBuffers.hpp>
#include <vulkan/CommandPool.hpp>
#include <vulkan/Device.hpp>
#include <vulkan/GraphicsPipeline.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/SwapChain.hpp>

#include <stdexcept>

using namespace vulkan;

CommandBuffers::CommandBuffers( const Device& device,
								const RenderPass& render_pass,
								const SwapChain& swap_chain,
								const GraphicsPipeline& graphical_pipeline,
								const CommandPool& command_pool )
	: m_device( device ),
	m_render_pass( render_pass ),
	m_swap_chain( swap_chain ),
	m_graphicsPipeline( graphical_pipeline ),
	m_command_pool( command_pool )
{
	createCommandBuffers();
}

void CommandBuffers::recreate()
{
	destroyCommandBuffers();
	createCommandBuffers();
}

void CommandBuffers::createCommandBuffers()
{
	m_commandBuffers.resize( m_render_pass.size() );

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_command_pool.handle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if( vkAllocateCommandBuffers( m_device.logical(), &allocInfo, m_commandBuffers.data() ) != VK_SUCCESS )
		throw std::runtime_error( "failed to allocate command buffers!" );

	for( size_t i = 0; i < m_commandBuffers.size(); i++ )
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if( vkBeginCommandBuffer( m_commandBuffers[i], &beginInfo ) != VK_SUCCESS )
			throw std::runtime_error( "failed to begin recording command buffer!" );

		VkRenderPassBeginInfo render_passInfo{};
		render_passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_passInfo.renderPass = m_render_pass.handle();
		render_passInfo.framebuffer = m_render_pass.frameBuffer( i );
		render_passInfo.renderArea.offset = { 0, 0 };
		render_passInfo.renderArea.extent = m_swap_chain.extent();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		render_passInfo.clearValueCount = 1;
		render_passInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass( m_commandBuffers[i], &render_passInfo, VK_SUBPASS_CONTENTS_INLINE );
		vkCmdBindPipeline( m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline() );
		vkCmdDraw( m_commandBuffers[i], 3, 1, 0, 0 );
		vkCmdEndRenderPass( m_commandBuffers[i] );

		if( vkEndCommandBuffer( m_commandBuffers[i] ) != VK_SUCCESS )
			throw std::runtime_error( "failed to record command buffer!" );
	}
}



CommandBuffers::~CommandBuffers()
{
	destroyCommandBuffers();
}

void CommandBuffers::destroyCommandBuffers()
{
	vkFreeCommandBuffers( m_device.logical(),
						  m_command_pool.handle(),
						  static_cast<uint32_t>( m_commandBuffers.size() ),
						  m_commandBuffers.data() );
}

void CommandBuffers::SingleTimeCommands( const Device& device,
										 const CommandPool& cmdPool,
										 const std::function<void( const VkCommandBuffer& )>& func )
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = cmdPool.handle();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer = {};
	if( vkAllocateCommandBuffers( device.logical(), &allocInfo, &commandBuffer ) != VK_SUCCESS )
		throw std::runtime_error( "failed to allocate command buffers!" );

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS )
		throw std::runtime_error( "Could not create one-time command buffer!" );

	func( commandBuffer );

	if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
		throw std::runtime_error( "failed to record command buffer!" );

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit( device.graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE );
	vkQueueWaitIdle( device.graphicsQueue() );

	vkFreeCommandBuffers( device.logical(), cmdPool.handle(), 1, &commandBuffer );
}