#pragma once
#include <vulkan/vulkan.h>
#include "common/non_copyable.hpp"
#include <vector>

namespace vulkan
{
class Device;
class SwapChain;

class RenderPass : public NonCopyable
{
public:
	RenderPass( const Device& device, const SwapChain& swap_chain );
	~RenderPass();

	inline const VkRenderPass& handle() const
	{
		return m_render_pass;
	}
	inline const VkFramebuffer& frameBuffer( uint32_t index ) const
	{
		return m_frameBuffers[index];
	}
	inline size_t size() const
	{
		return m_frameBuffers.size();
	}

	void recreate();
	void cleanupOld();

protected:
	VkRenderPass m_render_pass;
	VkRenderPass m_oldRenderPass;
	std::vector<VkFramebuffer> m_frameBuffers;
	const Device& m_device;
	const SwapChain& m_swap_chain;

	void createRenderPass();
	void createFrameBuffers();

	void destroyFrameBuffers();
};
}  // namespace vulkan
