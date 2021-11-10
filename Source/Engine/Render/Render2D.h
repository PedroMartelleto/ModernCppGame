#pragma once

#include "../Commons.h"
#include "Texture.h"
#include "Shader.h"
#include "Window.h"

#define RENDER2D_MAX_TEXTURES 32

struct Vertex
{
	Vec3f position;
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

class Render2D
{
public:
	static const size_t MaxQuadCount = 1024;
	static const size_t MaxVertexCount = MaxQuadCount * 4;
	static const size_t MaxIndexCount = MaxQuadCount * 6;
	static const size_t MaxTextures = RENDER2D_MAX_TEXTURES;

	static Shader* shader;
	static Matrix4f orthoMatrix;

	static void Create(Window* window);
	static void Destroy();

	static void BeginRender(const Matrix4f& viewMatrix = Matrix4f(1.0f));

	static void BeginBatch();
	static void EndBatch();
	static void Flush(const Matrix4f& transformMatrix = Matrix4f(1.0f));

	inline static float GetScreenWidth() { return (float)s_window->GetWidth(); }
	inline static float GetScreenHeight() { return (float)s_window->GetHeight(); }

	static void Resize(float width, float height);

	static void DrawRect(const Vec2f& pos, float angle, const Vec2f& size, int z, Ref<Texture> texture, const Color4f& color = Colors::WHITE);
	static void DrawRect(const Vec2f& pos, float angle, const Vec2f& size, int z = 0.0f, const Color4f& color = Colors::WHITE);
	static void DrawRect(const Vec2f& pos, float angle, const Vec2f& size, int z, const Rect2D& region, Ref<Texture> texture, const Color4f& color = Colors::WHITE);

	inline static void AddVertex(const Vec3f& position, const Color4f& color, const Vec2f& texCoords, float texIndex)
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
public:
	static Window* s_window;
	static RendererData s_data;
};

