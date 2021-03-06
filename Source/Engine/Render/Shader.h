// Small abstraction layer for dealing with shaders. Code adapted from https://github.com/BennyQBD/3DEngineCpp.

#pragma once
#include "../Commons.h"

class RenderingEngine;
class DirectionalLight;
class PointLight;
class SpotLight;

class TypedData
{
public:
	TypedData(const std::string& name, const std::string& type) :
		m_name(name),
		m_type(type) {}

	inline const std::string& GetName() const { return m_name; }
	inline const std::string& GetType() const { return m_type; }
private:
	std::string m_name;
	std::string m_type;
};

class UniformStruct
{
public:
	UniformStruct(const std::string& name, const std::vector<TypedData>& memberNames) :
		m_name(name),
		m_memberNames(memberNames) {}

	inline const std::string& GetName() const { return m_name; }
	inline const std::vector<TypedData>& GetMemberNames() const { return m_memberNames; }
private:
	std::string            m_name;
	std::vector<TypedData> m_memberNames;
};

class ShaderData
{
public:
	ShaderData(const std::string& fileName);
	virtual ~ShaderData();

	inline int GetProgram()                                           const { return m_program; }
	inline const std::vector<int>& GetShaders() const { return m_shaders; }
	inline const RBTree<std::string, unsigned int>& GetUniformMap() const { return m_uniformMap; }
private:
	void AddVertexShader(const std::string& text);
	void AddGeometryShader(const std::string& text);
	void AddFragmentShader(const std::string& text);
	void AddProgram(const std::string& text, int type);

	void AddAllAttributes(const std::string& vertexShaderText, const std::string& attributeKeyword);
	void AddShaderUniforms();
	void AddUniform(const std::string& uniformName, const std::string& uniformType, const std::vector<UniformStruct>& structs);
	void CompileShader() const;

	static int s_supportedOpenGLLevel;
	static std::string s_glslVersion;
	int m_program;
	std::vector<int> m_shaders;
	RBTree<std::string, unsigned int> m_uniformMap;
};

class Shader
{
public:
	Shader(const std::string& fileName = "basicShader");
	Shader(const Shader& other);
	virtual ~Shader();

	void Bind() const;

	GLuint GetProgramID() const { return (GLuint)m_shaderData->GetProgram(); }

	void SetUniformi(const std::string& uniformName, int value) const;
	void SetUniformf(const std::string& uniformName, float value) const;
	void SetUniformMatrix4f(const std::string& uniformName, const Matrix4f& value) const;
	void SetUniformVec3f(const std::string& uniformName, const Vec3f& value) const;

	inline unsigned int Unif(const std::string& uniformName) const
	{
		return m_shaderData->GetUniformMap().at(uniformName);
	}

	inline unsigned int operator[](const std::string& uniformName) const
	{
		return m_shaderData->GetUniformMap().at(uniformName);
	}

	inline unsigned int operator[](const char* uniformName) const
	{
		return m_shaderData->GetUniformMap().at(std::string(uniformName));
	}
protected:
private:
	static std::map<std::string, ShaderData*> s_resourceMap;

	ShaderData* m_shaderData;
	std::string m_fileName;

	void operator=(const Shader& other) {}
};