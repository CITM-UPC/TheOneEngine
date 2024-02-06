#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>

struct Shader {
	unsigned int id;
	std::string vertex_shader;
	std::string fragment_shader;

	void CompileShader();
};

namespace ShaderUtils
{
	std::string default_vertex_shader;
	std::string default_fragment_shader;
}


#endif