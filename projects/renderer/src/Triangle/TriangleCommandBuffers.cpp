//
//#include <vulkan/Triangle/TriangleCommandBuffers.hpp>
//
//#include <stdexcept>
//
//using namespace vulkan;
//
//BasicCommandBuffers::BasicCommandBuffers( const Device& device,
//										  const RenderPass& render_pass,
//										  const SwapChain& swap_chain,
//										  const GraphicsPipeline& graphicsPipeline,
//										  const CommandPool& command_pool )
//	: CommandBuffers( device, render_pass, swap_chain, command_pool ),
//	m_graphicsPipeline( graphicsPipeline )
//{
//	createCommandBuffers();
//}
//
//void BasicCommandBuffers::recreate()
//{
//	destroyCommandBuffers();
//	createCommandBuffers();
//}
//
//void BasicCommandBuffers::createCommandBuffers()
//{
//	m_commandBuffers.resize( m_render_pass.size() );
//
//	VkCommandBufferAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.commandPool = m_command_pool.handle();
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();
//
//	if( vkAllocateCommandBuffers( m_device.logical(), &allocInfo, m_commandBuffers.data() ) != VK_SUCCESS )
//		throw std::runtime_error( "failed to allocate command buffers!" );
//
//	for( size_t i = 0; i < m_commandBuffers.size(); i++ )
//	{
//		VkCommandBufferBeginInfo beginInfo{};
//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//		if( vkBeginCommandBuffer( m_commandBuffers[i], &beginInfo ) != VK_SUCCESS )
//			throw std::runtime_error( "failed to begin recording command buffer!" );
//
//		VkRenderPassBeginInfo render_passInfo{};
//		render_passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//		render_passInfo.renderPass = m_render_pass.handle();
//		render_passInfo.framebuffer = m_render_pass.frameBuffer( i );
//		render_passInfo.renderArea.offset = { 0, 0 };
//		render_passInfo.renderArea.extent = m_swap_chain.extent();
//
//		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
//		render_passInfo.clearValueCount = 1;
//		render_passInfo.pClearValues = &clearColor;
//
//		vkCmdBeginRenderPass( m_commandBuffers[i], &render_passInfo, VK_SUBPASS_CONTENTS_INLINE );
//		vkCmdBindPipeline( m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.pipeline() );
//		vkCmdDraw( m_commandBuffers[i], 3, 1, 0, 0 );
//		vkCmdEndRenderPass( m_commandBuffers[i] );
//
//		if( vkEndCommandBuffer( m_commandBuffers[i] ) != VK_SUCCESS )
//			throw std::runtime_error( "failed to record command buffer!" );
//	}
//}