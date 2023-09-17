#include <vulkan/ImGui/ImGuiCommandBuffers.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <iostream>

using namespace vulkan;

ImGuiCommandBuffers::ImGuiCommandBuffers( const Device& device,
										  const RenderPass& render_pass,
										  const SwapChain& swap_chain,
										  const GraphicsPipeline& graphical_pipeline,
										  const CommandPool& command_pool )
	: CommandBuffers( device, render_pass, swap_chain, graphical_pipeline, command_pool )
{
	createCommandBuffers();
}

void ImGuiCommandBuffers::recreate()
{
	destroyCommandBuffers();
	createCommandBuffers();
}

void ImGuiCommandBuffers::createCommandBuffers()
{
	m_commandBuffers.resize( m_render_pass.size() );

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_command_pool.handle();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>( m_commandBuffers.size() );

	if( vkAllocateCommandBuffers( m_device.logical(), &allocInfo, m_commandBuffers.data() ) != VK_SUCCESS )
		throw std::runtime_error( "Unable to allocate UI command buffers!" );
}

void ImGuiCommandBuffers::recordCommandBuffers( uint32_t bufferIdx )
{
	VkCommandBufferBeginInfo cmdBufferBegin = {};
	cmdBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBegin.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if( vkBeginCommandBuffer( m_commandBuffers[bufferIdx], &cmdBufferBegin ) != VK_SUCCESS )
		throw std::runtime_error( "Unable to start recording UI command buffer!" );

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRenderPassBeginInfo render_passBeginInfo = {};
	render_passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_passBeginInfo.renderPass = m_render_pass.handle();
	render_passBeginInfo.framebuffer = m_render_pass.frameBuffer( bufferIdx );
	render_passBeginInfo.renderArea.extent.width = m_swap_chain.extent().width;
	render_passBeginInfo.renderArea.extent.height = m_swap_chain.extent().height;
	render_passBeginInfo.clearValueCount = 1;
	render_passBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass( m_commandBuffers[bufferIdx], &render_passBeginInfo, VK_SUBPASS_CONTENTS_INLINE );

	// Grab and record the draw data for Dear Imgui
	ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), m_commandBuffers[bufferIdx] );

	// End and submit render pass
	vkCmdEndRenderPass( m_commandBuffers[bufferIdx] );

	if( vkEndCommandBuffer( m_commandBuffers[bufferIdx] ) != VK_SUCCESS )
		throw std::runtime_error( "Failed to record command buffers!" );
}