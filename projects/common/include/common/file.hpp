#pragma once

#include <vector>
#include <filesystem>

namespace vulkan
{

std::vector<char> LoadFile( const std::filesystem::path path );

}