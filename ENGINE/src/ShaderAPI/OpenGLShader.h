#pragma once
#include "ShaderAPI.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <d3d11.h>
#include <d3dcompiler.h>

// Function to read shader source code from a file
static std::string readShaderSource(const std::string& filePath)
{
	std::ifstream shaderFile(filePath);
	if (!shaderFile.is_open())
	{
		console.Log("Couldn't read shader file" + filePath, API::OPENGL, LEVEL::ERRORS);
		return "";
	}
	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();
	return shaderStream.str();
}

// Function to compile a shader and check for errors
static uint32_t CompileShader_Vertex(const char* source)
{
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		console.Log("Vertex shader compilation failed" + std::string(infoLog), API::OPENGL, LEVEL::ERRORS); // Log failure
		return 0;
	}

	console.Log("Vertex Shader compiled successfully", API::OPENGL, LEVEL::SUCCESS); // Log success
	return shader;
}

static uint32_t CompileShader_Frag(const char* source)
{
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		console.Log("Shader Fragment compilation failed" + std::string(infoLog), API::OPENGL, LEVEL::ERRORS); // Log failure
		return 0;
	}

	console.Log("Fragment Shader compiled successfully", API::OPENGL, LEVEL::SUCCESS); // Log success
	return shader;
}

// Function to link shaders into a program and check for errors
static uint32_t CreateShaderProgram(const char* vertex_shader_src, const char* fragment_shader_src)
{
	GLuint vertex_shader = CompileShader_Vertex(vertex_shader_src);
	GLuint fragment_shader = CompileShader_Frag(fragment_shader_src);

	// Create the shader program and attach the shaders
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	// Check for linking errors
	GLint success;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shader_program, 512, nullptr, infoLog);
		console.Log("Linking failed", API::OPENGL, LEVEL::ERRORS);
		return 0;
	}

	// Delete the shaders after linking
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	console.Log("Shader program created", API::SHADER, LEVEL::SUCCESS);

	return shader_program;
}

class OpenGLShader : public Shader
{
private:
	uint32_t Shader_ID;

	glm::mat4 projection_matrix = glm::mat4(1.0f);

public:
	OpenGLShader() {}

	~OpenGLShader()
	{
		if (Shader_ID)
			glDeleteProgram(Shader_ID);
	}

	void InitShader(const char path[], int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float min_distance, float max_distance, bool fullscreen = false) override
	{
		std::string vertex_shader_src_str = readShaderSource(std::string(path) + "/vert.glsl");
		const char* vertex_shader_src = vertex_shader_src_str.c_str();
		std::string fragment_shader_src_str = readShaderSource(std::string(path) + "/frag.glsl");
		const char* fragment_shader_src = fragment_shader_src_str.c_str();

		Shader_ID = CreateShaderProgram(vertex_shader_src, fragment_shader_src);

		if (Shader_ID == 0)
		{
			console.Log("Shader program creation failed!", API::OPENGL, LEVEL::ERRORS); // Log failure
			return;
		}

		console.Log("Shader program initialized successfully", API::OPENGL, LEVEL::SUCCESS); // Log success

		if (!fullscreen)
		{
			glUseProgram(Shader_ID);
			GLint projectionLoc = glGetUniformLocation(Shader_ID, "projection");
			projection_matrix = glm::perspective(glm::radians(FOV), float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), min_distance, max_distance);
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		}
		glUseProgram(0);
	}

	void UpdateProjection(int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float min_distance, float max_distance) override
	{
		glUseProgram(Shader_ID);
		GLint projectionLoc = glGetUniformLocation(Shader_ID, "projection");

		// Set up the projection matrix
		projection_matrix = glm::perspective(glm::radians(FOV), float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), min_distance, max_distance);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		//glUseProgram(0);
	}

	uint32_t GetShaderId()
	{
		return Shader_ID;
	}

	void Bind() override
	{
		glUseProgram(Shader_ID);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//std::cout << "Binding program: " << m_ProgramID << std::endl;
		//exit(0);
	}

	void Unbind() override
	{
		glUseProgram(0);
	}

	uint32_t GetUniformLocation(const std::string& name) override
	{
		uint32_t location = glGetUniformLocation(Shader_ID, name.c_str());
		return location;
	}

	void SetUniformMatrix4(const std::string& name, const float* matrix) override
	{
		GLint location = glGetUniformLocation(Shader_ID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
	}

	void SetUniformFloat(const std::string& name, float value) override
	{
		GLint location = glGetUniformLocation(Shader_ID, name.c_str());
		glUniform1f(location, value);
	}

	void SetUniformVec3(const std::string& name, const float* vector) override
	{
		GLint location = glGetUniformLocation(Shader_ID, name.c_str());
		glUniform3fv(location, 1, vector);
	}
};
