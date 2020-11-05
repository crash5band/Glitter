#include "Shader.h"
#include <sstream>
#include <fstream>
#include <iostream>

Shader::Shader(const std::string& name, const char* v, const char* f)
{
	this->name = name;
	compile(v, f);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

std::string Shader::getName() const
{
	return name;
}

void Shader::compile(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode, fragmentCode;
	std::ifstream vertexFile, fragmentFile;
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);

		std::stringstream vertexStream, fragmnetStream;

		vertexStream << vertexFile.rdbuf();
		fragmnetStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmnetStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "SHADER::COMPILE: failed to read shader source\n" << e.what() << '\n';
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char log[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, log);
		std::cout << "SHADER::VERTEX::COMPILATION_FAILED\n";
		std::cout << log;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, log);
		std::cout << "SHADER::FRAGMENT::COMPILATION_FAILED\n";
		std::cout << log;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, log);
		std::cout << "SHADER::PROGRAM::LINKING_FAILED\n";
		std::cout << log;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, DirectX::XMVECTOR value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, (GLfloat*)&value);
}

void Shader::setVec3(const std::string& name, DirectX::XMVECTOR value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, (GLfloat*)&value);
}

void Shader::setVec4(const std::string& name, DirectX::XMVECTOR value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, (GLfloat*)&value);
}

void Shader::setMatrix4(const std::string& name, DirectX::XMMATRIX value) const
{
	auto loc = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&value.r->m128_f32[0]);
}