#pragma once
#include <vulkan/vulkan.h>
#include "common/non_copyable.hpp"

namespace vulkan
{
class Device;

class CommandPool : public NonCopyable
{
public:
	CommandPool( const Device& device, const VkCommandPoolCreateFlags& flags );
	~CommandPool();

	inline const VkCommandPool& handle() const
	{
		return m_pool;
	};

private:
	VkCommandPool m_pool;
	VkCommandPoolCreateFlags m_flags;

	const Device& m_device;
};
}  // namespace vulkan
