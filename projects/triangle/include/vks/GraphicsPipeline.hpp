
#pragma once

#include <vulkan/vulkan.h>
#include <NonCopyable.hpp>
#include <vector>
#include <filesystem>

namespace vks
{

class Device;
class SwapChain;
class RenderPass;
struct ShaderDetails;

class GraphicsPipeline : public NonCopyable
{
public:
	GraphicsPipeline( const Device& device, const SwapChain& swapChain, const RenderPass& renderPass );
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
	const SwapChain& m_swapChain;
	const RenderPass& m_renderPass;

	std::vector<char> LoadShaderFromFile( const std::filesystem::path vert );
	//void createPipeline( const std::filesystem::path vert, const std::filesystem::path frag );
	void createPipeline();
	VkShaderModule createShaderModule( const std::vector<unsigned char>& code );
};
}  // namespace vks
