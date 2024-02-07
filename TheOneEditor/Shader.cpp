#include "Shader.h"
#include "GL/glew.h"
#include "Log.h"

namespace ShaderUtils
{
	int CreateVertexShader(unsigned int& vertex_id, const char* vertex_shader) {
		int success = GL_FALSE;

		vertex_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_id, 1, &vertex_shader, NULL);
		glCompileShader(vertex_id);
		glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &success);

		return success;
	}
	int CreateFragmentShader(unsigned int& fragment_id, const char* fragment_shader) {
		int success = GL_FALSE;

		fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_id, 1, &fragment_shader, NULL);
		glCompileShader(fragment_id);
		glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &success);

		return success;
	}
}

Shader::~Shader()
{
	if (glIsProgram(id)) {
		glDeleteProgram(id);
		id = 0;
	}
}

void Shader::CompileShader()
{
    int success = GL_FALSE;
	bool failed = false;
	const int k_log_length = 512;
	char error_log[k_log_length];

	success = ShaderUtils::CreateVertexShader(vertex_id, vertex_shader.c_str());
	if (success == GL_FALSE) {
		glGetShaderInfoLog(vertex_id, k_log_length, NULL, error_log);
		LOG(LogType::LOG_ERROR, "Failed to compile vertex shader, error %s", error_log);
		glDeleteShader(vertex_id);
		return;
	}

	success = ShaderUtils::CreateFragmentShader(fragment_id, fragment_shader.c_str());
	if (success == GL_FALSE) {
		glGetShaderInfoLog(fragment_id, k_log_length, NULL, error_log);
		LOG(LogType::LOG_ERROR, "Failed to compile fragment shader, error %s", error_log);
		glDeleteShader(fragment_id);
		glDeleteShader(vertex_id);
		return;
	}

	id = glCreateProgram();
	glAttachShader(id, vertex_id);
	glAttachShader(id, fragment_id);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		glGetProgramInfoLog(id, k_log_length, NULL, error_log);
		LOG(LogType::LOG_ERROR, "Failed to create shader program: %s", error_log);
	}

	// These are now compiled so we can delete them
	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
}
