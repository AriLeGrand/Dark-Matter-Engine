#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ConsoleManager.h"
#include "SceneAPI.h"

#include "ModelAPI/ModelInfo.h"
#include "ModelAPI/OpenGLModel.h"

#include "PostProccessAPI/OpenGLPostProccess.h"

#include "ShaderAPI/OpenGLShader.h"

class OpenGLScene : public Scene
{
private:
	std::vector<OpenGLModel> models;
	std::vector<ModelInfo*> modelsinfo;
	uint32_t BaseShaderProgramId;
	uint32_t FinnalPassShaderProgramId;

	std::vector<uint32_t> PostProccessPasses;

	uint32_t sceneTexture;
	uint32_t normalTexture;
	uint32_t depthTexture;
	uint32_t sceneFBO;

	uint32_t sceneFBO_final;
	uint32_t depthTexture_final;
	uint32_t normalTexture_final;
	uint32_t sceneTexture_final;

	uint32_t noisetexture;

	uint32_t passfbo;
	uint32_t passTexture;

	uint32_t quadVAO;
	uint32_t quadVBO;

	std::vector<std::string> modelName;

public:
	OpenGLScene(uint32_t ShaderProgramId, uint32_t FinnalPassShaderProgramID)
	{
		BaseShaderProgramId = ShaderProgramId;
		FinnalPassShaderProgramId = FinnalPassShaderProgramID;

		SetupRenderTargets(1280, 720);
		console.Log("OpenGLScene created", API::OPENGL, LEVEL::SUCCESS);
	};
	~OpenGLScene() {};

	void AddModel(const char path[], const char texture_path[]) override
	{
		OpenGLModel _;
		_.Load(path, texture_path, BaseShaderProgramId);
		//_.SetProgram(BaseShaderProgramId);
		_.GetInfo()->ShaderProgramId = BaseShaderProgramId;
		models.push_back(_);
		modelsinfo.push_back(_.GetInfo());
	}

	void RemoveModel(int IDX) override
	{
		if (IDX >= 0 && IDX < models.size())
			models.erase(models.begin() + IDX);
	}

	int GetModelCount() override
	{
		return models.size();
	}

	ModelInfo* GetModelInfoAtIdx(int IDX) override
	{
		return models.at(IDX).GetInfo();
	}

	std::vector<ModelInfo*> GetModelsInfo() override
	{
		return modelsinfo;
	}

	void AddPostProccessPass(const char path[], int ScreenWidth, int ScreenHeight) override
	{
		OpenGLShader* _ = new OpenGLShader();
		_->InitShader(path, ScreenWidth, ScreenHeight, 45.0f, 0.1f, 100.0f);

		PostProccessPasses.push_back(_->GetShaderId());
		//FinnalPassShaderProgram.UpdateProjection(ScreenWidth, ScreenHeight, 45.0f, 0.1f, 100.0f);
	}

	void RemovePostProccessPassAtIdx(int Idx) override
	{
		PostProccessPasses.erase(PostProccessPasses.begin() + Idx);
	}

	uint32_t GetRenderTarget() override
	{
		return sceneFBO_final;
	}

	std::string GetSceneModelsName()
	{
		std::string name;
		for (auto& model : models)
		{
			name += model.GetModelName();
			return name;
		}
	}

	int GetSceneTriangleCount()
	{
		int _ = 0;
		for (auto& model : models)
		{
			_ += model.GetInfo()->TriangleCount / 3;
			return _;
		}
	}

	int GetSceneVerticesCount()
	{
		int _ = 0;
		for (auto& model : models)
		{
			_ += model.GetVerticesCount();
			return _;
		}
	}

	void SetupRenderTargets(int ScreenWidth, int ScreenHeight)
	{
		// Create the scene texture (color)
		//GLuint sceneTexture;
		glGenTextures(1, &sceneTexture);
		glBindTexture(GL_TEXTURE_2D, sceneTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ScreenWidth, ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create the normal texture
		//GLuint normalTexture;
		glGenTextures(1, &normalTexture);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create the depth texture
		//GLuint depthTexture;
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, ScreenWidth, ScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create and bind the framebuffer
		//GLuint sceneFBO;
		glGenFramebuffers(1, &sceneFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

		// Attach color, normal, and depth textures
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		// Specify multiple draw buffers (color + normal)
		GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, drawBuffers); // Enables writing to both textures

		// Ensure framebuffer is complete
		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "ERROR: Framebuffer is not complete! Status: " << std::hex << fboStatus << std::endl;
		}

		// Unbind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Generate random normalized vectors
		std::vector<GLfloat> randomData(ScreenWidth * ScreenHeight * 3); // 3 channels (RGB)
		std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random number generator

		for (int i = 0; i < ScreenWidth * ScreenHeight * 3; i += 3)
		{
			// Generate random components in the range [-1, 1]
			float x = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
			float y = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;
			float z = static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f;

			// Normalize the vector
			float length = std::sqrt(x * x + y * y + z * z);
			x /= length;
			y /= length;
			z /= length;

			// Store the normalized vector
			randomData[i] = x;
			randomData[i + 1] = y;
			randomData[i + 2] = z;
		}

		// Create and upload the texture
		//GLuint textureID;
		glGenTextures(1, &noisetexture);
		glBindTexture(GL_TEXTURE_2D, noisetexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, randomData.data());

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//// 1. Generate the framebuffer
		//glGenFramebuffers(1, &passfbo);
		//glBindFramebuffer(GL_FRAMEBUFFER, passfbo);

		//// 2. Create the color attachment texture
		//glGenTextures(1, &passTexture);
		//glBindTexture(GL_TEXTURE_2D, passTexture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); // Create a texture with RGBA format
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//// 3. Attach the texture to the framebuffer
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, passTexture, 0);

		//// 4. Check if the framebuffer is complete
		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		//    std::cerr << "Error: Framebuffer is not complete!" << std::endl;
		//}

		//// Unbind the framebuffer (optional, depending on your rendering flow)
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//GLuint sceneTexture;
		glGenTextures(1, &sceneTexture_final);
		glBindTexture(GL_TEXTURE_2D, sceneTexture_final);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ScreenWidth, ScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create the normal texture
		//GLuint normalTexture;
		glGenTextures(1, &normalTexture_final);
		glBindTexture(GL_TEXTURE_2D, normalTexture_final);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create the depth texture
		//GLuint depthTexture;
		glGenTextures(1, &depthTexture_final);
		glBindTexture(GL_TEXTURE_2D, depthTexture_final);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, ScreenWidth, ScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create and bind the framebuffer
		//GLuint sceneFBO;
		glGenFramebuffers(1, &sceneFBO_final);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO_final);

		// Attach color, normal, and depth textures
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture_final, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture_final, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture_final, 0);

		// Specify multiple draw buffers (color + normal)
		GLenum drawBuffers_[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, drawBuffers_); // Enables writing to both textures

		// Ensure framebuffer is complete
		GLenum fboStatus_ = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus_ != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "ERROR: Framebuffer is not complete! Status: " << std::hex << fboStatus_ << std::endl;
		}

		// Unbind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glUseProgram(FinnalPassShaderProgramId);
		// Quad vertices: Positions (x, y), Texture Coordinates (u, v)
		float quadVertices[] = {
			// x     y       u     v
			-1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
			-1.0f, 1.0f, 0.0f, 1.0f, // Top-left
			-1.0f, 1.0f, 0.0f, 1.0f, // Top-left
			1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
			1.0f, 1.0f, 1.0f, 1.0f // Top-right
		};

		//GLuint quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		// Position attribute (layout location = 0)
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute (layout location = 1)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Draw() override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO); // Bind the custom framebuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (auto& model : models)
		{
			model.Draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the custom framebuffer

		for (auto& pass : PostProccessPasses)
		{
			glUseProgram(pass);
			glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO_final); // Bind the custom framebuffer
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::vec3 cameraPosition = {0.0f, 0.0f, 5.0f};
			glm::quat cameraRotation = glm::quat(glm::radians(glm::vec3(0, 0, 0)));
			GLint modelLoc = glGetUniformLocation(pass, "model");
			GLint viewLoc = glGetUniformLocation(pass, "view");

			// Set up camera properties (position, rotation)
			glm::mat4 view_matrix = glm::mat4(1.0f); // Identity matrix
			glm::vec3 cameraFront = glm::normalize(cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f));
			glm::vec3 cameraUp = glm::normalize(cameraRotation * glm::vec3(0.0f, 1.0f, 0.0f));

			// Camera position and view matrix
			view_matrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

			glm::mat4 model_matrix = glm::mat4(1.0f); // Identity matrix (no transformation)
			// Set the final model matrix to the shader
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

			glActiveTexture(GL_TEXTURE0); // Texture unit 0
			glBindTexture(GL_TEXTURE_2D, sceneTexture);
			glUniform1i(glGetUniformLocation(pass, "sceneTexture"), 0); // Set "sceneTexture" uniform to 0

			// Activate and bind the depth texture to texture unit 1
			glActiveTexture(GL_TEXTURE1); // Texture unit 1
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glUniform1i(glGetUniformLocation(pass, "depthMap"), 1); // Set "depthMap" uniform to 1

			// Activate and bind the depth texture to texture unit 2
			glActiveTexture(GL_TEXTURE2); // Texture unit 2
			glBindTexture(GL_TEXTURE_2D, normalTexture);
			glUniform1i(glGetUniformLocation(pass, "normalMap"), 2); // Set "depthMap" uniform to 2

			//// Activate and bind the depth texture to texture unit 3
			//glActiveTexture(GL_TEXTURE3); // Texture unit 3
			//glBindTexture(GL_TEXTURE_2D, noisetexture);
			//glUniform1i(glGetUniformLocation(pass, "randomTexture"), 3); // Set "depthMap" uniform to 2

			glDisable(GL_DEPTH_TEST); // Don't use depth testing for UI
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6); // Draw the quad using 6 vertices
			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the custom framebuffer
		}

		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO); // Bind the custom framebuffer
		glUseProgram(FinnalPassShaderProgramId);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::vec3 cameraPosition = {0.0f, 0.0f, 5.0f};
		glm::quat cameraRotation = glm::quat(glm::radians(glm::vec3(0, 0, 0)));
		GLint modelLoc = glGetUniformLocation(FinnalPassShaderProgramId, "model");
		GLint viewLoc = glGetUniformLocation(FinnalPassShaderProgramId, "view");

		// Set up camera properties (position, rotation)
		glm::mat4 view_matrix = glm::mat4(1.0f); // Identity matrix
		glm::vec3 cameraFront = glm::normalize(cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 cameraUp = glm::normalize(cameraRotation * glm::vec3(0.0f, 1.0f, 0.0f));

		// Camera position and view matrix
		view_matrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		glm::mat4 model_matrix = glm::mat4(1.0f); // Identity matrix (no transformation)
		// Set the final model matrix to the shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glActiveTexture(GL_TEXTURE0); // Texture unit 0
		glBindTexture(GL_TEXTURE_2D, sceneTexture_final);
		glUniform1i(glGetUniformLocation(FinnalPassShaderProgramId, "sceneTexture"), 0); // Set "sceneTexture" uniform to 0

		// Activate and bind the depth texture to texture unit 1
		glActiveTexture(GL_TEXTURE1); // Texture unit 1
		glBindTexture(GL_TEXTURE_2D, depthTexture_final);
		glUniform1i(glGetUniformLocation(FinnalPassShaderProgramId, "depthMap"), 1); // Set "depthMap" uniform to 1

		// Activate and bind the depth texture to texture unit 2
		glActiveTexture(GL_TEXTURE2); // Texture unit 2
		glBindTexture(GL_TEXTURE_2D, normalTexture_final);
		glUniform1i(glGetUniformLocation(FinnalPassShaderProgramId, "normalMap"), 2); // Set "depthMap" uniform to 2

		glDisable(GL_DEPTH_TEST); // Don't use depth testing for UI
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6); // Draw the quad using 6 vertices
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the custom framebuffer

		//GLenum error = glGetError();
		//if (glGetError() != GL_NO_ERROR) {
		//    std::cerr << "OpenGL Error: " << error << std::endl;
		//}
	}
};
