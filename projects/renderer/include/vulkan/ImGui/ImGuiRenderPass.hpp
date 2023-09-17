#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vulkan/RenderPass.hpp>

namespace vulkan
{
class ImGuiRenderPass : public RenderPass
{
public:
	ImGuiRenderPass( const Device& device, const SwapChain& swap_chain );

private:
	void createRenderPass();
};
}  // namespace vulkan
