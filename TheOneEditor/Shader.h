#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>

struct Shader {
	~Shader();

	unsigned int id = 0;
	unsigned int vertex_id = 0;
	unsigned int fragment_id = 0;
	std::string vertex_shader;
	std::string fragment_shader;

	void CompileShader();
};


#endif