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

namespace ShaderUtils
{
	// Simple Vertex Shader
	const char* default_vertex_shader = 
		R"(#version 330 core
		layout (location = 0) in vec3 a_Position;
		layout (location = 1) in vec2 a_TexCoord;
		
		uniform vec4 u_Model;
		
		out vec2 v_texCoord;
		
		void main() {
			gl_Position = u_Model * vec4(a_Position, 1.0f)
			v_TexCoord = a_TexCoord;
		})";

	// Simple Fragment Shader
	const char* default_fragment_shader =
		R"(#version 330 core
		uniform sampler3D u_Texture;
		
		in vec2 v_TexCoord;
		out vec4 color;
		
		void main() {
			color = texture(u_Texture, v_TexCoord);
		})"
}


#endif