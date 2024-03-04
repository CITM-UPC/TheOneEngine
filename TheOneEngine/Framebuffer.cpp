#include "Framebuffer.h"
#include "Log.h"

#include "GL/glew.h"

static const unsigned int s_MaxFramebufferSize = 8192;

FrameBuffer::FrameBuffer(int newWidth, int newHeight, bool depth) : initialized(true), depthActive(depth)
{
	width = newWidth;
	height = newHeight;

	Reset(depth);
}

FrameBuffer::~FrameBuffer()
{
	if (initialized) {
		glDeleteTextures(1, &colorAttachment);
		glDeleteTextures(1, &depthAttachment);
		glDeleteFramebuffers(1, &FBO);
	}
}

void FrameBuffer::Bind(bool clear)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, width, height);
	if (clear) {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Reset(bool depth)
{
	if (FBO)
	{
		glDeleteTextures(1, &colorAttachment);
		glDeleteTextures(1, &depthAttachment);
		glDeleteFramebuffers(1, &FBO);

		colorAttachment = 0;
		depthAttachment = 0;
	}

	// FRAMEBUFFER
	glCreateFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Color texture
	glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

	if (depth) {
		// Depth attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment);
		glBindTexture(GL_TEXTURE_2D, depthAttachment);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
	}

	// Check framebuffer status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(LogType::LOG_WARNING, "Framebuffer not completed");
	}
	else
		LOG(LogType::LOG_OK, "Framebuffer completed");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(unsigned int newWidth, unsigned int newHeight)
{
	if (newWidth < 0 || newHeight < 0 || newWidth > s_MaxFramebufferSize || newHeight > s_MaxFramebufferSize)
	{
		LOG(LogType::LOG_INFO, "Attempted to rezize framebuffer to %d, %d", width, height);
		return;
	}
	width = newWidth;
	height = newHeight;

	Reset(depthActive);
}

void FrameBuffer::Clear(glm::vec4 color)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FrameBuffer::ClearBuffer(int value)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearTexImage(colorAttachment, 0, GL_RGBA8, GL_INT, &value);
}

