#pragma once
#include "ShaderAPI.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>
#include <vector>

class Dx11Shader : public Shader
{
private:
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_InputLayout = nullptr;

	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11Device* d3d11Device = nullptr;

	uint32_t Shader_ID;

	ID3D10Blob* VS_Buffer = nullptr;
	ID3D10Blob* PS_Buffer = nullptr;
	ID3D10Blob* errorBuffer = nullptr;

public:
	Dx11Shader() = default;

	~Dx11Shader()
	{
		if (m_VertexShader)
			m_VertexShader->Release();
		if (m_PixelShader)
			m_PixelShader->Release();
		if (VS_Buffer)
			VS_Buffer->Release();
		if (PS_Buffer)
			PS_Buffer->Release();
		if (errorBuffer)
			errorBuffer->Release();
	}

	//std::vector<std::unique_ptr<Dx11Shader>> shaders;

	void InitShader(const char path[], int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float min_distance = 0.1f, float max_distance = 100.0f, bool fullscreen = false) override
	{
		// Log the initialization process
		console.Log("Initializing shader", API::SHADER, LEVEL::INFO);

		std::string shaderPath = std::string(path) + "\\vertex-shader.hlsl";
		std::wstring wname(shaderPath.begin(), shaderPath.end());

		console.Log("Trying to load: " + shaderPath, API::SHADER, LEVEL::INFO);

		// Vertex Shader Compilation
		HRESULT hr = D3DCompileFromFile(wname.c_str(), nullptr, nullptr, "VS", "vs_4_0", D3DCOMPILE_DEBUG, 0, &VS_Buffer, &errorBuffer);
		if (FAILED(hr) || !VS_Buffer)
		{
			if (errorBuffer)
			{
				console.Log("Vertex Shader compilation failed! " + std::string((const char*)errorBuffer->GetBufferPointer()), API::SHADER, LEVEL::ERRORS);
				errorBuffer->Release();
			}
			return;
		}

		// Pixel Shader Compilation
		hr = D3DCompileFromFile(wname.c_str(), nullptr, nullptr, "PS", "ps_4_0", D3DCOMPILE_DEBUG, 0, &PS_Buffer, &errorBuffer);
		if (FAILED(hr) || !PS_Buffer)
		{
			if (errorBuffer)
			{
				console.Log("Pixel Shader compilation failed! " + std::string((const char*)errorBuffer->GetBufferPointer()), API::SHADER, LEVEL::ERRORS);
				errorBuffer->Release();
			}
			return;
		}

		// Shader Creation
		hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_VertexShader);
		hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_PixelShader);
		if (FAILED(hr))
		{
			console.Log("Shader creation failed!", API::SHADER, LEVEL::ERRORS);
			return;
		}

		// Set Shaders
		deviceContext->VSSetShader(m_VertexShader, 0, 0);
		deviceContext->PSSetShader(m_PixelShader, 0, 0);

		console.Log("Shader successfully loaded: " + std::string(path), API::SHADER, LEVEL::SUCCESS);
	}

	void SetDxDeviceContext(ID3D11Device* dxDevice, ID3D11DeviceContext* d3d11DevCon)
	{
		d3d11Device = dxDevice;
		deviceContext = d3d11DevCon;
	}

	ID3D10Blob* GetVSBuffer()
	{
		return VS_Buffer;
	}

	virtual void UpdateProjection(int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float min_distance = 0.1f, float max_distance = 100.0f) override {}

	uint32_t GetShaderId() override
	{
		return Shader_ID;
	}

	void Bind() override
	{
		deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_PixelShader, nullptr, 0);
	}

	void Unbind() override
	{
		deviceContext->VSSetShader(nullptr, nullptr, 0);
		deviceContext->PSSetShader(nullptr, nullptr, 0);
	}

	uint32_t GetUniformLocation(const std::string& name) override
	{
		return 0;
	}

	void SetUniformMatrix4(const std::string& name, const float* matrix) override {}
	void SetUniformFloat(const std::string& name, float value) override {}
	void SetUniformVec3(const std::string& name, const float* vector) override {}
};
