#include "vulkan/Shader.hpp"

namespace vulkan
{
Shader::Shader( std::vector<unsigned char> compiled_shader, Type type )
	: mType( type ),
	mData( std::move( compiled_shader ) )
{}

Shader::Shader( std::vector<unsigned char>&& compiled_shader, Type type )
	: mType( type ),
	  mData( std::move( compiled_shader ) )
{}

Shader::Shader( const std::filesystem::path& path, Type type )
{
}

std::vector<unsigned char> Shader::GetShaderData()
{
	return mData;
}

}