#include "Render2D.h"

RendererData Render2D::s_data = RendererData();
Shader* Render2D::shader = nullptr;
Matrix4f Render2D::orthoMatrix = Matrix4f(1.0f);
Window* Render2D::s_window = nullptr;

const int MAX_Z = 100000;

void Render2D::Create(Window* window)
{
	s_window = window;

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	assert(s_data.quadBuffer == nullptr);

	// Inits the dynamic vertex buffer
	s_data.quadBuffer = new Vertex[MaxVertexCount];

	glCreateVertexArrays(1, &s_data.vao);
	glBindVertexArray(s_data.vao);

	glCreateBuffers(1, &s_data.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, s_data.vbo);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));

	// Creates static index buffer
	uint32_t* indices = new uint32_t[MaxIndexCount];
	uint32_t offset = 0;

	for (int i = 0; i < MaxIndexCount; i += 6)
	{
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}

	glCreateBuffers(1, &s_data.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * MaxIndexCount, indices, GL_STATIC_DRAW);

	delete[] indices;

	// 1x1 white texture for rendering solids
	glCreateTextures(GL_TEXTURE_2D, 1, &s_data.whiteTextureID);
	glBindTexture(GL_TEXTURE_2D, s_data.whiteTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	
	Render2D::AssignTexture(s_data.whiteTextureID, 0);
	s_data.textureCount = 1;

	for (size_t i = 1; i < MaxTextures; ++i)
	{
		s_data.textureIndexToID[i] = 0;
	}

	shader = new Shader("Resources/Shaders/Shader.glsl");

	shader->Bind();

	int samplers[MaxTextures];

	for (int i = 0; i < MaxTextures; ++i) 
	{
		samplers[i] = i;
	}

	glUniform1iv(glGetUniformLocation(shader->GetProgramID(), "u_Textures"), MaxTextures, &samplers[0]);

	Resize(GetScreenWidth(), GetScreenHeight());
}

void Render2D::Resize(float width, float height)
{
	orthoMatrix = glm::ortho(0.0f, width, height, 0.0f, -0.01f, 100.0f);
	glViewport(0, 0, (int)width, (int)height);
}

void Render2D::BeginRender(const Matrix4f& viewMatrix)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->Bind();
	shader->SetUniformMatrix4f("u_ViewProj", viewMatrix * orthoMatrix);
}

void Render2D::Destroy()
{
	glDeleteVertexArrays(1, &s_data.vao);
	glDeleteBuffers(1, &s_data.vbo);
	glDeleteBuffers(1, &s_data.ibo);

	glDeleteTextures(1, &s_data.whiteTextureID);

	delete shader;
	delete[] s_data.quadBuffer;
}

void Render2D::BeginBatch()
{
	// Resets "current position"
	s_data.quadBufferPtr = s_data.quadBuffer;
}

void Render2D::EndBatch()
{
	// Gets the number of bytes allocated
	GLsizeiptr size = (uint8_t*)s_data.quadBufferPtr - (uint8_t*)s_data.quadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, s_data.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_data.quadBuffer);
}

void Render2D::Flush(const Matrix4f& transformMatrix)
{
	for (uint32_t i = 0; i < s_data.textureCount; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, s_data.textureIndexToID[i]);
	}

	shader->SetUniformMatrix4f("u_Transform", transformMatrix);
	glBindVertexArray(s_data.vao);
	glDrawElements(GL_TRIANGLES, s_data.indexCount, GL_UNSIGNED_INT, nullptr);
	
	// Resets batch data
	s_data.indexCount = 0;
	s_data.textureCount = 1;
	s_data.textureIDToIndex.clear();
	s_data.textureIDToIndex[s_data.textureIndexToID[0]] = 0;
}

void Render2D::DrawRect(const Vec2f& pos, const Vec2f& size, int z, Ref<Texture> texture, const Color4f& color)
{
	DrawRect(pos, size, z, Rect2D(0, 0, texture->GetWidth(), texture->GetHeight()), texture, color);
}

void Render2D::DrawRect(const Vec2f& pos, const Vec2f& size, int z, const Color4f& color)
{
	DrawRect(pos, size, z, Rect2D(0, 0, 1, 1), nullptr, color);
}

void Render2D::DrawRect(const Vec2f& pos, const Vec2f& size, int z, const Rect2D& region, Ref<Texture> texture, const Color4f& color)
{
	// If our current batch is too big...
	if (s_data.indexCount >= MaxIndexCount)
	{
		// Submit it and start a new one
		EndBatch();
		Flush();
		BeginBatch();
	}

	float texIndex = 0.0f;
	auto norm = Rect2D(region);

	if (texture != nullptr)
	{
		norm.pos.x /= texture->GetWidth();
		norm.size.x /= texture->GetWidth();
		norm.pos.y /= texture->GetHeight();
		norm.size.y /= texture->GetHeight();

		auto textureID = texture->GetID();

		// Checks if the texture id is already in the dictionary
		if (s_data.textureIDToIndex.find(textureID) != s_data.textureIDToIndex.end())
		{
			texIndex = (float) s_data.textureIDToIndex[textureID];
		}
		else
		{
			if (s_data.textureCount >= MaxTextures)
			{
				// Submit it and start a new one
				EndBatch();
				Flush();
				BeginBatch();
			}

			// Otherwise add it to the dictionary
			AssignTexture(textureID, s_data.textureCount);
			texIndex = (float) s_data.textureCount;
			s_data.textureCount += 1;
		}
	}

	float zFloat = -(1.0f - (float)z / (float)MAX_Z) * 50.0f;
	Vec2f offset = Vec2f(norm.size.x < 0.0f ? -norm.size.x : 0.0f, norm.size.y < 0.0f ? -norm.size.y : 0.0f);

	AddVertex(Vec3f(pos.x, pos.y, zFloat), color, norm.pos + offset, texIndex);
	AddVertex(Vec3f(pos.x + size.x, pos.y, zFloat), color, Vec2f(norm.pos.x + norm.size.x, norm.pos.y) + offset, texIndex);
	AddVertex(Vec3f(pos.x + size.x, pos.y + size.y, zFloat), color, norm.pos + norm.size + offset, texIndex);
	AddVertex(Vec3f(pos.x, pos.y + size.y, zFloat), color, Vec2f(norm.pos.x, norm.pos.y + norm.size.y) + offset, texIndex);

	s_data.indexCount += 6;
}