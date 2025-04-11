#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <windows.h>

#include "InputManager.h"
#include "RendererAPI.h"

#include "SceneAPI/Dx11Scene.h"

#include "ShaderAPI/Dx11Shader.h"

//Map storing the key and the state of the key (pressed or not)
#include "imgui/backends/imgui_impl_win32.h"

#include "UIAPI/UIAPI.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	InputManager* pInputManager = reinterpret_cast<InputManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEWHEEL:
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	//Pass messages to default handler for any other messages
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

class DX11Renderer : public RendererAPI
{
private:
	//ID3D11DeviceContext* d3dContext = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	HINSTANCE hInstance = nullptr;
	LPCTSTR WndClassName = nullptr;
	HWND hwnd;
	HRESULT hr;

	std::vector<Dx11Scene> scenes;
	int CurrentSceneIdx = -1;

	std::vector<Dx11Shader> shaders;
	Dx11Shader FinnalPassShader;

	int ScreenWidth = 0;

	int ScreenHeight = 0;

	LARGE_INTEGER frequency;
	LARGE_INTEGER lastTime;

	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;

	D3D11_INPUT_ELEMENT_DESC layout[3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Position (x, y, z)
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Normal (nx, ny, nz)
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // UV (u, v)
	};

	UINT numElements = ARRAYSIZE(layout);
	ID3D11InputLayout* vertLayout = nullptr;

public:
	bool window_is_opened = false;

	void Init() override
	{
		// Initialize Direct3D 11 (create device, context, swap chain, etc.)
		hInstance = GetModuleHandle(NULL);
		WndClassName = L"win";
		console.Log("Renderer Initialized", API::RENDERER, LEVEL::SUCCESS);
	}

	Dx11Scene* GetCurrentScene()
	{
		return &scenes.at(CurrentSceneIdx);
	}

	void AddScene() override
	{
		Dx11Scene _(GetDefaultShaderId(), FinnalPassShader.GetShaderId());
		_.SetDxDeviceContext(device, deviceContext, VS_Buffer);
		scenes.push_back(_);
		if (CurrentSceneIdx == -1)
		{
			CurrentSceneIdx = 0;
		}
		console.Log("Scene added", API::RENDERER, LEVEL::SUCCESS);
	}

	void RemoveScene(int IDX) override {}

	void Shutdown() override
	{
		// Clean up Direct3D resources
		// Clean up Direct3D resources
		if (renderTargetView)
		{
			renderTargetView->Release();
			renderTargetView = nullptr;
		}
		if (swapChain)
		{
			swapChain->Release();
			swapChain = nullptr;
		}
		if (device)
		{
			device->Release();
			device = nullptr;
		}
		if (deviceContext)
		{
			deviceContext->Release();
			deviceContext = nullptr;
		}
		shaders.clear();

		// Clean up window resources
		if (hwnd)
		{
			DestroyWindow(hwnd);
			hwnd = nullptr;
		}

		if (hInstance)
		{
			UnregisterClassW(WndClassName, hInstance);
			hInstance = nullptr;
		}

		console.Log("DX11Renderer Shutdown complete", API::RENDERER, LEVEL::SUCCESS);
	}

	void LoadShader(const char path[]) override
	{
		Dx11Shader _;
		_.SetDxDeviceContext(device, deviceContext);
		_.InitShader(path, ScreenWidth, ScreenHeight, 45.0f, 0.1f, 100.0f);
		VS_Buffer = _.GetVSBuffer();
		shaders.push_back(_);
		console.Log("Loaded Shader from: " + std::string(path), API::SHADER, LEVEL::INFO);
	}

	void SetFinnalPassShader(const char path[]) override
	{
		FinnalPassShader.SetDxDeviceContext(device, deviceContext);
		FinnalPassShader.InitShader(path, ScreenWidth, ScreenHeight, 45.0f, 0.1f, 1000.0f);
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
			console.Log("Tried to access nonexistent shader program, returning default...", API::SHADER, LEVEL::WARNING);
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
			console.Log("Tried to bind inexisting shader... Switching to default", API::SHADER, LEVEL::WARNING);
			shaders.at(0).Bind();
			return;
		}
		shaders.at(IDX).Bind();
	}

	bool createWindow(int width, int height, const char* name) override
	{
		const char* suffix = " [DX11]";
		std::string buff(name);
		buff.append(suffix);
		const char* window_name = buff.c_str();

		std::wstring wname(window_name, window_name + strlen(window_name)); // Convert char* to std::wstring

		WNDCLASSEXW wc = {}; // Use WNDCLASSEXW for Unicode
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
		wc.lpszClassName = WndClassName;
		wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		RegisterClassExW(&wc);

		hwnd = CreateWindowExW(0, WndClassName, wname.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);

		if (!hwnd)
		{
			// Replace MessageBoxW with console logging
			console.Log("Error creating window", API::UI, LEVEL::ERRORS);
			return false;
		}

		ShowWindow(hwnd, 1);
		UpdateWindow(hwnd);

		// Init 3D

		// Describe our Buffer
		DXGI_MODE_DESC bufferDesc;

		ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

		bufferDesc.Width = width;
		bufferDesc.Height = height;
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Describe our SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Create our SwapChain (maybe problem here)
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);

		if (FAILED(hr))
		{
			console.Log("Failed to create swap chain", API::RENDERER, LEVEL::ERRORS);
			return false;
		}

		// Create our BackBuffer
		ID3D11Texture2D* BackBuffer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

		if (FAILED(hr))
		{
			console.Log("Failed to get buffer from swap chain", API::RENDERER, LEVEL::ERRORS);
			return false;
		}

		// Create our Render Target
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		hr = device->CreateRenderTargetView(BackBuffer, &rtvDesc, &renderTargetView);
		if (FAILED(hr))
		{
			console.Log("Failed to create render target view", API::RENDERER, LEVEL::ERRORS);
			BackBuffer->Release();
			return false;
		}
		BackBuffer->Release();

		// Set our Render Target
		deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);
		window_is_opened = true;
		ScreenWidth = width;
		ScreenHeight = height;

		console.Log("Window created successfully", API::UI, LEVEL::SUCCESS);

		return true;
	}

	void Clear(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) override
	{
		float color[] = {r, g, b, a};
		deviceContext->ClearRenderTargetView(renderTargetView, color);
	}

	void DrawIndexed(uint32_t indexCount) override
	{
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	void SetViewport(int x, int y, int width, int height) override
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = static_cast<float>(x);
		viewport.TopLeftY = static_cast<float>(y);
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		deviceContext->RSSetViewports(1, &viewport);
	}

	bool isWindowOpened() override
	{
		return window_is_opened;
	}

	void Present() override
	{
		MSG msg = {};

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			window_is_opened = (msg.message != WM_QUIT);
			DispatchMessage(&msg);
		}
		swapChain->Present(0, 0);
	}

	bool InitUI() override
	{
		console.Log("Initializing UI", API::UI, LEVEL::INFO);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		if (!ImGui_ImplWin32_Init(hwnd))
		{
			console.Log("Failed to initialize ImGui for Win32", API::UI, LEVEL::ERRORS);
			return false;
		}

		if (!ImGui_ImplDX11_Init(device, deviceContext))
		{
			console.Log("Failed to initialize ImGui for DX11", API::UI, LEVEL::ERRORS);
			return false;
		}

		console.Log("UI initialized successfully", API::UI, LEVEL::SUCCESS);
		return true;
	}

	void DrawUI() override
	{
		QueryPerformanceFrequency(&frequency); // Get the frequency once
		QueryPerformanceCounter(&lastTime); // Set initial time
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
		frameCount++;

		// Update FPS every 1 second
		if (deltaTime >= 1.0f)
		{
			fps = frameCount;
			frameCount = 0;
			lastTime = currentTime; // Reset timer
		}

		Dx11Scene* scene = GetCurrentScene();
		modelCount = scene->GetModelCount();
		modelString = scene->GetModelsName();
		triangleCount = scene->GetSceneTriangleCount();
		verticesCount = scene->GetSceneVerticesCount();
		mouseX = inputManager.GetMousePosition().first;
		mouseY = inputManager.GetMousePosition().second;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		MainUI(NULL, ScreenWidth, ScreenHeight);

		uint32_t texture = scene->GetRenderTarget();
		ImGui::Begin("FBO Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
		ImGui::Image((uintptr_t)texture, ImVec2(ScreenWidth / 2, ScreenHeight / 2), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)); // Adjust size as necessary
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ClearUI() override
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		console.Log("UI cleaned", API::UI, LEVEL::SUCCESS);
	}
};
