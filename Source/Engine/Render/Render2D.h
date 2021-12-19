#pragma once

#include "../Commons.h"
#include "Texture.h"
#include "Shader.h"
#include "Window.h"
#include "Font.h"

#define RENDER2D_MAX_TEXTURES 2

struct Vertex
{
	Vec2f position;
	Color4f color;
	Vec2f texCoords;
	float texIndex;
};

/// <summary>
/// Data used internally by the renderer.
/// </summary>
struct RendererData
{
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;

	GLuint whiteTextureID = 0;
	const uint32_t whiteTextureSlot = 0;

	uint32_t indexCount = 0;

	Vertex* quadBuffer = nullptr;
	Vertex* quadBufferPtr = nullptr;

	// Maps a texture index to an opengl texture id
	std::array<uint32_t, RENDER2D_MAX_TEXTURES> textureIndexToID;
	HashMap<GLuint, uint32_t> textureIDToIndex;
	uint32_t textureCount = 1;
};

struct Render2DDrawData
{
	std::array<Vec2f, 4> pos;
	std::array<Vec2f, 4> texCoords;
	float z;
	Ref<Texture> texture;
	Color4f color;

	inline bool operator<(const Render2DDrawData& other)
	{
		return z < other.z;
	}

	inline bool operator>=(const Render2DDrawData& other)
	{
		return z >= other.z;
	}
};

class Render2D
{
public:
	inline static const float MAX_Z = 10.0f;
	inline static const float NEAR_PLANE = 0.002f;
	inline static const float FAR_PLANE = MAX_Z + 1.0f;

	static const size_t MaxQuadCount = 4096;
	static const size_t MaxVertexCount = MaxQuadCount * 4;
	static const size_t MaxIndexCount = MaxQuadCount * 6;
	static const size_t MaxTextures = RENDER2D_MAX_TEXTURES;

	static Shader* shader;
	static Matrix4f orthoMatrix;

	static void Create(Window* window);
	static void Destroy();

	static void BeginRender(const Matrix4f& viewMatrix = Matrix4f(1.0f));
	static void DrawQueued();

	static void SetClearColor(const Color4f& clearColor);

	static void BeginBatch();
	static void EndBatch();
	static void Flush(const Matrix4f& transformMatrix = Matrix4f(1.0f));

	inline static float GetScreenWidth() { return (float)s_window->GetWidth(); }
	inline static float GetScreenHeight() { return (float)s_window->GetHeight(); }
	inline static Vec2f GetScreenSize() { return Vec2f((float)s_window->GetWidth(), (float)s_window->GetHeight()); }
	inline static Vec2f GetScreenCenter() { return GetScreenSize() / 2.0f; }

	static void Resize(float width, float height);

	static void DrawLine(bool arrow, const Vec2f& from, const Vec2f& to, float z, float width, const Color4f& color = Colors::WHITE);

	static void DrawRect(const Vec2f& pos, float angle, const Vec2f& size, float z, const Ref<Texture>& texture, const Color4f& color = Colors::WHITE);
	static void DrawRect(const Vec2f& pos, float angle, const Vec2f& size, float z = 0.0f, const Color4f& color = Colors::WHITE);
	static void DrawRect(const Vec2f& pos, float angle, const Vec2f& size, float z, const Rect2D& region, const Ref<Texture>& texture, const Color4f& color = Colors::WHITE);
	static void DrawQuad(const std::array<Vec2f, 4>& pos, const std::array<Vec2f, 4> texCoords, const Ref<Texture>& texture, const Color4f& color = Colors::WHITE);

	static void DrawLineOfText(const Vec2f& center, float scale, const std::string& text, float z, const Ref<Font>& font, const Color4f& color = Colors::WHITE);

	static Vec2f GetTextSize(float scale, const std::string& text, const Ref<Font>& font);

	static void EnqueueDrawQuad(const std::array<Vec2f, 4>& pos, const std::array<Vec2f, 4> texCoords, float z, const Ref<Texture>& texture, const Color4f& color = Colors::WHITE);

	inline static void AddVertex(const Vec2f& position, const Color4f& color, const Vec2f& texCoords, float texIndex)
	{
		s_data.quadBufferPtr->position = position;
		s_data.quadBufferPtr->color = color;
		s_data.quadBufferPtr->texCoords = texCoords;
		s_data.quadBufferPtr->texIndex = texIndex;
		s_data.quadBufferPtr++; // Go to the next vertex
	}
private:
	inline static void AssignTexture(GLuint textureID, int index)
	{
		s_data.textureIndexToID[index] = textureID;
		s_data.textureIDToIndex[textureID] = index;
	}

	static std::map<float, Render2DDrawData> s_drawOrder;
public:
	static Window* s_window;
	static RendererData s_data;
};

