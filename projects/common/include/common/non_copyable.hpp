#pragma once

namespace vulkan
{
class NonCopyable
{
public:
	NonCopyable() = default;
	NonCopyable( const NonCopyable& ) = delete;
	NonCopyable& operator=( const NonCopyable& ) = delete;
};
}