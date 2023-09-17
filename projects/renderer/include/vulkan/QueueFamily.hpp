#pragma once
#include <vulkan/vulkan.h>
#include <optional>

namespace vulkan {
  struct QueueFamilyIndices {
    // Drawing command support
    std::optional<uint32_t> graphicsFamily;
    // Support for drawing to surface
    std::optional<uint32_t> presentFamily;

    inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
  };

  class QueueFamily {
  public:
    QueueFamily() = delete;
    ~QueueFamily() = delete;

    static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device,
                                                const VkSurfaceKHR& surface);
  };
}  // namespace vulkan
