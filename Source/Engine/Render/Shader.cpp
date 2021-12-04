// Small abstraction layer for dealing with shaders. Code adapted from https://github.com/BennyQBD/3DEngineCpp.

#include "Shader.h"


std::map<std::string, ShaderData*> Shader::s_resourceMap;
int ShaderData::s_supportedOpenGLLevel = 0;
std::string ShaderData::s_glslVersion = "";

static std::string LoadShader(const std::string& fileName);


ShaderData::ShaderData(const std::string& fileName)
{
	std::string actualFileName = fileName;
#if PROFILING_DISABLE_SHADING != 0
	actualFileName = "nullShader";
#endif

	m_program = glCreateProgram();

	if (m_program == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	if (s_supportedOpenGLLevel == 0)
	{
		int majorVersion;
		int minorVersion;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		s_supportedOpenGLLevel = majorVersion * 100 + minorVersion * 10;

		if (s_supportedOpenGLLevel >= 330)
		{
			std::ostringstream convert;
			convert << s_supportedOpenGLLevel;

			s_glslVersion = convert.str();
		}
		else if (s_supportedOpenGLLevel >= 320)
		{
			s_glslVersion = "150";
		}
		else if (s_supportedOpenGLLevel >= 310)
		{
			s_glslVersion = "140";
		}
		else if (s_supportedOpenGLLevel >= 300)
		{
			s_glslVersion = "130";
		}
		else if (s_supportedOpenGLLevel >= 210)
		{
			s_glslVersion = "120";
		}
		else if (s_supportedOpenGLLevel >= 200)
		{
			s_glslVersion = "110";
		}
		else
		{
			fprintf(stderr, "Error: OpenGL Version %d.%d does not support shaders.\n", majorVersion, minorVersion);
			exit(1);
		}
	}

	std::string shaderText = LoadShader(actualFileName);

	std::string vertexShaderText = "#version " + s_glslVersion + "\n#define VS_BUILD\n#define GLSL_VERSION " + s_glslVersion + "\n" + shaderText;
	std::string fragmentShaderText = "#version " + s_glslVersion + "\n#define FS_BUILD\n#define GLSL_VERSION " + s_glslVersion + "\n" + shaderText;

	AddVertexShader(vertexShaderText);
	AddFragmentShader(fragmentShaderText);

	std::string attributeKeyword = "attribute";
	AddAllAttributes(vertexShaderText, attributeKeyword);

	CompileShader();

	AddShaderUniforms();
}

ShaderData::~ShaderData()
{
	for (std::vector<int>::iterator it = m_shaders.begin(); it != m_shaders.m_end(); ++it)
	{
		glDetachShader(m_program, *it);
		glDeleteShader(*it);
	}
	glDeleteProgram(m_program);
}

Shader::Shader(const std::string& fileName)
{
	m_fileName = fileName;

	std::map<std::string, ShaderData*>::const_iterator it = s_resourceMap.find(fileName);
	if (it != s_resourceMap.m_end())
	{
		m_shaderData = it->second;
	}
	else
	{
		m_shaderData = new ShaderData(fileName);
		s_resourceMap.insert(std::pair<std::string, ShaderData*>(fileName, m_shaderData));
	}
}

Shader::Shader(const Shader& other) :
	m_shaderData(other.m_shaderData),
	m_fileName(other.m_fileName)
{}

Shader::~Shader()
{
	if (m_shaderData)
	{
		if (m_fileName.length() > 0)
			s_resourceMap.erase(m_fileName);

		delete m_shaderData;
	}
}


void Shader::Bind() const
{
	glUseProgram(m_shaderData->GetProgram());
}


void Shader::SetUniformi(const std::string& uniformName, int value) const
{
	glUniform1i(m_shaderData->GetUniformMap().at(uniformName), value);
}

void Shader::SetUniformf(const std::string& uniformName, float value) const
{
	glUniform1f(m_shaderData->GetUniformMap().at(uniformName), value);
}

void Shader::SetUniformVec3f(const std::string& uniformName, const Vec3f& value) const
{
	glUniform3f(m_shaderData->GetUniformMap().at(uniformName), value.x, value.y, value.z);
}

void Shader::SetUniformMatrix4f(const std::string& uniformName, const Matrix4f& value) const
{
	glUniformMatrix4fv(m_shaderData->GetUniformMap().at(uniformName), 1, GL_FALSE, &(value[0][0]));
}

void ShaderData::AddVertexShader(const std::string& text)
{
	AddProgram(text, GL_VERTEX_SHADER);
}

void ShaderData::AddGeometryShader(const std::string& text)
{
	AddProgram(text, GL_GEOMETRY_SHADER);
}

void ShaderData::AddFragmentShader(const std::string& text)
{
	AddProgram(text, GL_FRAGMENT_SHADER);
}

void ShaderData::AddProgram(const std::string& text, int type)
{
	int shader = glCreateShader(type);

	if (shader == 0)
	{
		fprintf(stderr, "Error creating shader type %d\n", type);
		exit(1);
	}

	const GLchar* p[1];
	p[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];

		glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader, InfoLog);

		exit(1);
	}

	glAttachShader(m_program, shader);
	m_shaders.push_back(shader);
}

void ShaderData::AddAllAttributes(const std::string& vertexShaderText, const std::string& attributeKeyword)
{
	int currentAttribLocation = 0;
	size_t attributeLocation = vertexShaderText.find(attributeKeyword);
	while (attributeLocation != std::string::npos)
	{
		bool isCommented = false;
		size_t lastLineEnd = vertexShaderText.rfind("\n", attributeLocation);

		if (lastLineEnd != std::string::npos)
		{
			std::string potentialCommentSection = vertexShaderText.substr(lastLineEnd, attributeLocation - lastLineEnd);

			// Potential false positives are both in comments, and in macros.
			isCommented = potentialCommentSection.find("//") != std::string::npos || potentialCommentSection.find("#") != std::string::npos;
		}

		if (!isCommented)
		{
			size_t begin = attributeLocation + attributeKeyword.length();
			size_t m_end = vertexShaderText.find(";", begin);

			std::string attributeLine = vertexShaderText.substr(begin + 1, m_end - begin - 1);

			begin = attributeLine.find(" ");
			std::string attributeName = attributeLine.substr(begin + 1);

			glBindAttribLocation(m_program, currentAttribLocation, attributeName.c_str());
			currentAttribLocation++;
		}
		attributeLocation = vertexShaderText.find(attributeKeyword, attributeLocation + attributeKeyword.length());
	}
}

void ShaderData::AddShaderUniforms()
{
	GLint numBlocks;
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &numBlocks);
	
	std::vector<std::string> nameList;
	nameList.reserve(numBlocks);

	GLchar* name = new GLchar[2048];

	for (int i = 0; i < numBlocks; ++i)
	{
		int length = 0, size = 0;
		GLenum type = 0;

		glGetActiveUniform(m_program, (GLuint)i, 2048, &length, &size, &type, name);

		auto unifName = Utils::StringSplit(std::string(name), "[")[0];
		m_uniformMap[unifName] = glGetUniformLocation(m_program, unifName.c_str());
		assert(m_uniformMap[unifName] >= 0);
	}

	delete[] name;
}

void ShaderData::AddUniform(const std::string& uniformName, const std::string& uniformType, const std::vector<UniformStruct>& structs)
{
	bool addThis = true;

	for (unsigned int i = 0; i < structs.size(); i++)
	{
		if (structs[i].GetName().compare(uniformType) == 0)
		{
			addThis = false;
			for (unsigned int j = 0; j < structs[i].GetMemberNames().size(); j++)
			{
				AddUniform(uniformName + "." + structs[i].GetMemberNames()[j].GetName(), structs[i].GetMemberNames()[j].GetType(), structs);
			}
		}
	}

	if (!addThis)
	{
		return;
	}

	unsigned int location = glGetUniformLocation(m_program, uniformName.c_str());
	assert(location != GL_INVALID_VALUE);
	m_uniformMap.insert(std::pair<std::string, unsigned int>(uniformName, location));
}

static void CheckShaderError(int shader, int flag, bool isProgram, const std::string& errorMessage);

void ShaderData::CompileShader() const
{
	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Invalid shader program");
}

static void CheckShaderError(int shader, int flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (!success)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		fprintf(stderr, "%s: '%s'\n", errorMessage.c_str(), error);
	}
}

static std::string LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open(fileName.c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);

			if (line.find("#include") == std::string::npos)
				output.append(line + "\n");
			else
			{
				std::string includeFileName = Utils::StringSplit(line, " ")[1];
				includeFileName = includeFileName.substr(1, includeFileName.length() - 2);

				std::string toAppend = LoadShader(includeFileName);
				output.append(toAppend + "\n");
			}
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
};