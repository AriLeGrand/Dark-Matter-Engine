#pragma once
#include "RendererAPI.h"
#include "Dx11Renderer.h"
#include "OpenGLRenderer.h"
//#include "LogManager.h"

enum class RendererBackend
{
	OpenGL,
	DirectX11
};

enum RendererBackend ParseBackend(char* TargetBackend)
{
	if (strcmp(TargetBackend, "DX11") == 0)
		return RendererBackend::DirectX11;
	if (strcmp(TargetBackend, "OPENGL") == 0)
		return RendererBackend::OpenGL;

	console.Log("Defaulting to OPENGL", API::RENDERER, LEVEL::INFO);
	return RendererBackend::OpenGL;
}

class Renderer
{
private:
	static RendererAPI* s_RendererAPI;

public:
	static void Init(RendererBackend backend)
	{
		if (s_RendererAPI)
			delete s_RendererAPI;

		if (backend == RendererBackend::OpenGL)
		{
			s_RendererAPI = new OpenGLRenderer();
			console.Log("Init OPENGL", API::RENDERER, LEVEL::INFO);
		}
		else if (backend == RendererBackend::DirectX11)
		{
			s_RendererAPI = new DX11Renderer();
			console.Log("Init DX11", API::RENDERER, LEVEL::INFO);
		}

		s_RendererAPI->Init();
	}

	static void Shutdown()
	{
		if (s_RendererAPI)
		{
			s_RendererAPI->Shutdown();
			delete s_RendererAPI;
			s_RendererAPI = nullptr;
		}
	}

	static RendererAPI* Get()
	{
		return s_RendererAPI;
	}
};

// Global pointer to renderer instance
RendererAPI* Renderer::s_RendererAPI = nullptr;
