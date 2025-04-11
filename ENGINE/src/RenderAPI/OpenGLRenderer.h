#pragma once
#include <iostream>
#include <vector>
#include <windows.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "SceneAPI/OpenGLScene.h"
#include "ShaderAPI/OpenGLShader.h"
#include "RenderAPI/RendererAPI.h"

#include "AudioManager.h"

class OpenGLRenderer : public RendererAPI
{
private:
	GLFWwindow* window;

	std::vector<OpenGLShader> shaders;
	int ScreenWidth = 0;
	int ScreenHeight = 0;
	OpenGLShader FinnalPassShader;

	std::vector<OpenGLScene> scenes;
	int CurrentSceneIdx = -1;

	// UI related
	float lastTime = 0.0f; // Time of the last frame
	uint32_t fps = 0, frameCount = 0;
	uint32_t modelCount, triangleCount, verticesCount;
	uint32_t mouseX, mouseY;
	std::string modelString;

public:
	bool window_is_opened = false;

	void Init() override
	{
		// Initialize OpenGL (load extensions, set up state, etc.)
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		console.Log("OpenGL Initialization started", API::OPENGL, LEVEL::INFO);
	}

	OpenGLScene* GetCurrentScene()
	{
		return &scenes.at(CurrentSceneIdx);
	}

	void AddScene() override
	{
		OpenGLScene newScene(GetDefaultShaderId(), FinnalPassShader.GetShaderId());
		scenes.push_back(newScene);
		if (CurrentSceneIdx == -1)
			CurrentSceneIdx = 0;
		console.Log("Scene added", API::OPENGL, LEVEL::INFO);
	}

	void RemoveScene(int IDX) override
	{
		if (IDX < 0 || IDX >= scenes.size())
		{
			console.Log("Invalid scene index", API::OPENGL, LEVEL::ERRORS);
			return;
		}
		scenes.erase(scenes.begin() + IDX);
		console.Log("Scene removed", API::OPENGL, LEVEL::INFO);
	}

	void Shutdown() override
	{
		// Clean up OpenGL resources
		glfwDestroyWindow(window);
		glfwTerminate();
		console.Log("OpenGL resources cleaned up", API::OPENGL, LEVEL::INFO);
	}

	void SetFinnalPassShader(const char path[]) override
	{
		FinnalPassShader.InitShader(path, ScreenWidth, ScreenHeight, 45.0f, 0.1f, 1000.0f);
		console.Log("Final pass shader set", API::OPENGL, LEVEL::INFO);
	}

	void LoadShader(const char path[]) override
	{
		shaders.push_back(OpenGLShader{});
		shaders.at(shaders.size() - 1).InitShader(path, ScreenWidth, ScreenHeight, 60.0f, 0.1f, 1000.0f);
		console.Log("Shader loaded from: " + std::string(path), API::OPENGL, LEVEL::INFO);
	}

	std::vector<uint32_t> GetShadersIds() override
	{
		std::vector<uint32_t> Shaders_ids;
		for (auto& shader : shaders)
		{
			Shaders_ids.push_back(shader.GetShaderId());
		}
		return Shaders_ids;
	}

	uint32_t GetShaderIdByIdx(int Idx) override
	{
		if (Idx > shaders.size())
		{
			console.Log("Tried to access nonexistent shader program, returning default...", API::OPENGL, LEVEL::ERRORS);
			return GetDefaultShaderId();
		}
		return shaders.at(Idx).GetShaderId();
	}

	uint32_t GetDefaultShaderId() override
	{
		return shaders.at(0).GetShaderId();
	}

	void BindDefaultShader() override
	{
		shaders.at(0).Bind();
	}

	void BindShaderByIdx(int IDX) override
	{
		if (IDX > shaders.size())
		{
			console.Log("Tried to bind nonexistent shader... Switching to default", API::OPENGL, LEVEL::ERRORS);
			shaders.at(0).Bind();
			return;
		}
		shaders.at(IDX).Bind();
	}

	bool createWindow(int width, int height, const char* name) override
	{
		const char* suffix = " [OPENGL]";
		std::string buff(name);
		buff.append(suffix);
		const char* window_name = buff.c_str();
		window = glfwCreateWindow(width, height, window_name, NULL, NULL);
		if (window == NULL)
		{
			console.Log("Failed to create GLFW window", API::OPENGL, LEVEL::ERRORS);
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		gladLoadGL();
		glViewport(0, 0, width, height);
		ScreenWidth = width;
		ScreenHeight = height;
		window_is_opened = true;
		console.Log("Window created successfully", API::OPENGL, LEVEL::SUCCESS);
		return true;
	}

	void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f) override
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void DrawIndexed(uint32_t indexCount) override
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void SetViewport(int x, int y, int width, int height) override
	{
		glViewport(x, y, width, height);
	}

	bool isWindowOpened() override
	{
		return window_is_opened;
	}

	void Present() override
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
		window_is_opened = !glfwWindowShouldClose(window);
	}

	bool InitUI()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
		console.Log("UI successfully initialized", API::UI, LEVEL::SUCCESS);
		return true;
	}

	void DrawUI()
	{
		float currentTime = (float)glfwGetTime(); // Get the current time in seconds
		float deltaTime = currentTime - lastTime;
		frameCount++;

		if (currentTime - lastTime >= 1.0f)
		{
			fps = frameCount;
			frameCount = 0;
			lastTime = currentTime;
		}

		OpenGLScene* scene = GetCurrentScene();
		modelCount = scene->GetModelCount();
		modelString = scene->GetSceneModelsName();
		triangleCount = scene->GetSceneTriangleCount();
		verticesCount = scene->GetSceneVerticesCount();
		mouseX = inputManager.GetMousePosition().first;
		mouseY = inputManager.GetMousePosition().second;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		MainUI(NULL, ScreenWidth, ScreenHeight);

		uint32_t texture = scene->GetRenderTarget();
		ImGui::Begin("FBO Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
		ImGui::Image((uintptr_t)texture, ImVec2(ScreenWidth / 2, ScreenHeight / 2), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)); // Adjust size as necessary
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ClearUI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		console.Log("UI resources cleaned up", API::UI, LEVEL::INFO);
	}
};
