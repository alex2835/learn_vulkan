#pragma once
#include <vulkan/vulkan.h>
#include "common/non_copyable.hpp"
#include <vector>
#include <filesystem>
#include <span>

#include <vulkan/Shader.hpp>

#include "common/file.hpp"
#include "common/pointers.hpp"

namespace vulkan
{

class Device;
class SwapChain;
class RenderPass;
struct ShaderDetails;

class GraphicsPipeline : public NonCopyable
{
public:
	GraphicsPipeline( const Device& device,
					  const SwapChain& swap_chain,
					  const RenderPass& render_pass,
					  Shaders shaders );
	~GraphicsPipeline();

	void recreate();

	inline const VkPipeline& pipeline() const
	{
		return m_pipeline;
	}
	inline const VkPipelineLayout& layout() const
	{
		return m_layout;
	}

private:
	VkPipeline m_pipeline;
	VkPipelineLayout m_layout;
	VkPipelineLayout m_oldLayout;

	const Device& m_device;
	const SwapChain& m_swap_chain;
	const RenderPass& m_render_pass;
	Shaders mShaders;

	void createPipeline();
	VkShaderModule createShaderModule( const std::vector<unsigned char>& code );
};
}  // namespace vulkan
