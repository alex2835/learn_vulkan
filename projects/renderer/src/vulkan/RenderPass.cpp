
#include <iostream>
#include <vulkan/Device.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/SwapChain.hpp>

using namespace vulkan;

RenderPass::RenderPass( const Device& device, const SwapChain& swap_chain )
	: m_render_pass( VK_NULL_HANDLE ),
	m_oldRenderPass( VK_NULL_HANDLE ),
	m_device( device ),
	m_swap_chain( swap_chain )
{
	createRenderPass();
	createFrameBuffers();
}

RenderPass::~RenderPass()
{
	destroyFrameBuffers();
	vkDestroyRenderPass( m_device.logical(), m_render_pass, nullptr );
}

void RenderPass::recreate()
{
	destroyFrameBuffers();
	m_oldRenderPass = m_render_pass;
	createRenderPass();
	createFrameBuffers();
}

void RenderPass::cleanupOld()
{
	if( m_oldRenderPass != VK_NULL_HANDLE )
	{
		vkDestroyRenderPass( m_device.logical(), m_oldRenderPass, nullptr );
		m_oldRenderPass = VK_NULL_HANDLE;
	}
}

void RenderPass::createRenderPass()
{
	// Create a new render pass as a color attachment
	VkAttachmentDescription colorAttachment = {};
	// Format should match the format of the swap chain
	colorAttachment.format = m_swap_chain.imageFormat();
	// No multisampling
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// Clear data before rendering, then store result after
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// Not doing anything with stencils, so don't care about it
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// Don't care about initial layout, but final layout should
	// be same as the presentation source
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Post-rendering subpasses

	// Subpass attachment reference
	VkAttachmentReference colorRef = {};
	// Index of 0 in color attachments description array (we're only using 1)
	colorRef.attachment = 0;
	// Use the optimal layout for color attachments
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Subpass description
	VkSubpassDescription subpass = {};
	// Using for graphics computation
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// Attach the color attachment
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	// Subpass dependencies
	VkSubpassDependency dependency = {};
	// Implicit subpass
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	// Subpass depdency is in index 0
	dependency.dstSubpass = 0;
	// Wait for color attachment output before accessing image
	// This prevents the image being accessed by subpass and swap chain at the same time
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	// Prevent transistion from happening until after reading and writing of color attachment
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Create the render pass
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &colorAttachment;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	if( vkCreateRenderPass( m_device.logical(), &createInfo, nullptr, &m_render_pass ) != VK_SUCCESS )
		throw std::runtime_error( "Render pass creation failed" );
}

void RenderPass::createFrameBuffers()
{
	size_t numImages = m_swap_chain.numImages();
	m_frameBuffers.resize( numImages );

	// Create a framebuffer for each image view
	for( size_t i = 0; i < numImages; i++ )
	{
		VkImageView attachments[] = { m_swap_chain.imageView( i ) };

		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = m_render_pass;
		info.attachmentCount = 1;
		info.pAttachments = attachments;
		info.width = m_swap_chain.extent().width;
		info.height = m_swap_chain.extent().height;
		info.layers = 1;

		if( vkCreateFramebuffer( m_device.logical(), &info, nullptr, &m_frameBuffers[i] ) != VK_SUCCESS )
			throw std::runtime_error( "Framebuffer creation failed" );
	}
}

void RenderPass::destroyFrameBuffers()
{
	for( VkFramebuffer& fb : m_frameBuffers )
		vkDestroyFramebuffer( m_device.logical(), fb, nullptr );
}