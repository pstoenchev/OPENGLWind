#pragma once
#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <glad\glad.h>
#include <string>
#include <fstream>
#include <sstream>

class ShaderProgram
{
public:
	unsigned int ID;

	ShaderProgram(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void use();

	// utility uniform functions
	void setBool(const char* name, bool value) const;
	void setInt(const char* name, int value) const;
	void setFloat(const char* name, float value) const;
};

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// open files
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	std::stringstream vShaderStream, fShaderStream;
	// read file's buffer contents into streams
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	// close file handlers
	vShaderFile.close();
	fShaderFile.close();

	std::string vShaderCode = vShaderStream.str();
	std::string fShaderCode = fShaderStream.str();

	// 2. compile shaders
	int success;
	char infoLog[512];

	// vertex Shader
	unsigned vertex = glCreateShader(GL_VERTEX_SHADER);
	try
	{
		const GLchar* vertSrc = vShaderCode.c_str();
		glShaderSource(vertex, 1, &vertSrc, NULL);
		glCompileShader(vertex);
		// print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::stringstream err("Vertex shader compilation error: ");
			err << infoLog;
			throw std::exception(err.str().c_str());
		};

		// fragment Shader
		unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);
		try
		{
			const GLchar* fragSrc = fShaderCode.c_str();
			glShaderSource(fragment, 1, &fragSrc, NULL);
			glCompileShader(fragment);
			// print compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				std::stringstream err("Fragment shader compilation error: ");
				err << infoLog;
				throw std::exception(err.str().c_str());
			};

			// shader Program
			ID = glCreateProgram();
			try
			{
				glAttachShader(ID, vertex);
				glAttachShader(ID, fragment);
				glLinkProgram(ID);
				// print linking errors if any
				glGetProgramiv(ID, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(ID, 512, NULL, infoLog);
					std::stringstream err("Shader program link error: ");
					err << infoLog;
					throw std::exception(err.str().c_str());
				}

				glDeleteShader(vertex);
				glDeleteShader(fragment);
			}
			catch (...)
			{
				glDeleteProgram(ID);
				throw;
			}
		}
		catch (...)
		{
			glDeleteShader(fragment);
			throw;
		}
	}
	catch (...)
	{
		glDeleteShader(vertex);
		throw;
	}
}

void ShaderProgram::use()
{
	glUseProgram(ID);
}

#endif
