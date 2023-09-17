#pragma once

#include <vector>
#include <filesystem>

#include "common/pointers.hpp"

namespace vulkan
{

class Shader
{
public:
	enum class Type
	{
		Vert,
		Frag
	};

	Shader( std::vector<unsigned char> compiled_shader, Type type );
	Shader( std::vector<unsigned char>&& compiled_shader, Type type );
	Shader( const std::filesystem::path& path, Type type );

	std::vector<unsigned char> GetShaderData();

private:
	Type mType;
	std::vector<unsigned char> mData;
};

struct Shaders
{
	Ref<Shader> Vert;
	Ref<Shader> Frag;
};

}