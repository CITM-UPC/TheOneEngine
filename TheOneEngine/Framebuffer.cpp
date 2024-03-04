#include "Framebuffer.h"
#include "Defs.h"
#include "Log.h"

#include "GL/glew.h"

static const uint32_t s_MaxFramebufferSize = 8192;

FrameBuffer::FrameBuffer(int width, int height, bool depth) : m_Init(true), m_DepthActive(depth)
{
	m_Width = width;
	m_Height = height;

	Reset(depth);
}

FrameBuffer::~FrameBuffer()
{
	if (m_Init) {
		glDeleteTextures(1, &m_ColorBufferTexture);
		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteFramebuffers(1, &m_FBO);
	}
}

void FrameBuffer::Bind(bool clear)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, m_Width, m_Height);
	if (clear) {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Reset(bool depth)
{
	if (m_FBO)
	{
		glDeleteTextures(1, &m_ColorBufferTexture);
		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteFramebuffers(1, &m_FBO);

		m_ColorBufferTexture = 0;
		m_DepthAttachment = 0;
	}

	// FRAMEBUFFER
	glCreateFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// Color texture
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_ColorBufferTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBufferTexture, 0);

	if (depth) {
		// Depth attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
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

void FrameBuffer::Resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
	{
		LOG(LogType::LOG_INFO, "Attempted to rezize framebuffer to %d, %d", width, height);
		return;
	}
	m_Width = width;
	m_Height = height;

	Reset(m_DepthActive);
}

int FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	int pixelData;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
	return pixelData;
}

void FrameBuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::ClearBuffer(int value)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glClearTexImage(m_ColorBufferTexture, 0, GL_RGBA8, GL_INT, &value);
}

