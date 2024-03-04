#pragma once

class FrameBuffer {
private:
	uint32_t m_FBO = 0;
	uint32_t m_ColorBufferTexture;
	uint32_t m_DepthAttachment = 0;
	uint32_t m_Width, m_Height;

	bool m_DepthActive;
	bool m_Init;

	void Reset(bool depth);
public:
	FrameBuffer(int width, int height, bool depth);
	~FrameBuffer();

	void Bind(bool clear = true);
	void Unbind();

	void Resize(uint32_t width, uint32_t height);
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y);

	void Clear();
	void ClearBuffer(int value);


	inline int getWidth() { return m_Width; }
	inline int getHeight() { return m_Height; }

	inline uint32_t getColorBufferTexture() { return m_ColorBufferTexture; }
	inline uint32_t getDepthBufferTexture() { return m_DepthAttachment; }
};
