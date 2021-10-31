// Small abstraction layer for Textures. Code adapted from https://github.com/BennyQBD/3DEngineCpp.

#pragma once

#include "../Commons.h"

class TextureData
{
public:
	TextureData(GLenum textureTarget, int width, int height, int numTextures, unsigned char** data, GLfloat* filters, GLenum* internalFormat, GLenum* format, bool clamp, GLenum* attachments);

	void Bind(int textureNum) const;
	void BindAsRenderTarget() const;

	inline int GetWidth()  const { return m_width; }
	inline int GetHeight() const { return m_height; }

	virtual ~TextureData();
protected:
private:
	TextureData(TextureData& other) {}
	void operator=(TextureData& other) {}

	void InitTextures(unsigned char** data, GLfloat* filter, GLenum* internalFormat, GLenum* format, bool clamp);
	void InitRenderTargets(GLenum* attachments);

	GLuint* m_textureID = nullptr;
	GLenum m_textureTarget = 0;
	GLuint m_frameBuffer = 0;
	GLuint m_renderBuffer = 0;
	int m_numTextures = 0;
	int m_width = 0;
	int m_height = 0;
};

class Texture
{
public:
	Texture(const std::string& fileName, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_NEAREST_MIPMAP_NEAREST, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false, GLenum attachment = GL_NONE);
	Texture(int width = 0, int height = 0, unsigned char* data = 0, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_NEAREST_MIPMAP_NEAREST, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false, GLenum attachment = GL_NONE);
	Texture(const Texture& texture);
	void operator=(Texture texture);
	virtual ~Texture();

	void Bind(unsigned int unit = 0) const;
	void BindAsRenderTarget() const;

	inline int GetWidth()  const { return m_textureData->GetWidth(); }
	inline int GetHeight() const { return m_textureData->GetHeight(); }

	bool operator==(const Texture& texture) const { return m_textureData == texture.m_textureData; }
	bool operator!=(const Texture& texture) const { return !operator==(texture); }
protected:
private:
	static std::map<std::string, TextureData*> s_resourceMap;

	TextureData* m_textureData;
	std::string m_fileName;
};