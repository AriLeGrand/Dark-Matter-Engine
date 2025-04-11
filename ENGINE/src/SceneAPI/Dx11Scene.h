#pragma once
#include "SceneAPI/SceneAPI.h"
#include "ModelAPI/Dx11Model.h"
#include "ModelAPI/ModelInfo.h"
#include "PostProccessAPI/Dx11PostProccess.h"

class Dx11Scene : public Scene
{
private:
	std::vector<Dx11Model> models;
	std::vector<ModelInfo*> modelsinfo;

	uint32_t BaseShaderProgramId;
	uint32_t FinnalPassShaderProgramId;

	std::vector<Dx11PostProccess> PostProccessPasses;
	ID3D11DeviceContext* deviceContext = nullptr;

	ID3D11Texture2D* renderTargetTexture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;

	ID3D11Device* device;
	ID3D10Blob* VS_Buffer;

public:
	Dx11Scene(uint32_t ShaderProgramId, uint32_t FinnalPassShaderProgramID)
	{
		BaseShaderProgramId = ShaderProgramId;
		FinnalPassShaderProgramId = FinnalPassShaderProgramID;
		console.Log("Scene created!", API::DX11, LEVEL::INFO);
	};

	~Dx11Scene()
	{
		if (renderTargetTexture)
			renderTargetTexture->Release();
		if (renderTargetView)
			renderTargetView->Release();
		if (device)
			device->Release();
		if (VS_Buffer)
			VS_Buffer->Release();
	};

	void SetDxDeviceContext(ID3D11Device* dxDevice, ID3D11DeviceContext* d3d11DevCon, ID3D10Blob* DxVS_Buffer)
	{
		deviceContext = d3d11DevCon;
		device = dxDevice;
		VS_Buffer = DxVS_Buffer;
	}

	void AddModel(const char path[], const char texture_path[]) override
	{
		Dx11Model _;
		_.SetDxDeviceContext(device, deviceContext, VS_Buffer);
		_.Load(path, texture_path, BaseShaderProgramId);
		//_.SetProgram(BaseShaderProgramId);
		_.GetInfo()->ShaderProgramId = BaseShaderProgramId;

		models.push_back(_);
		console.Log("Model added", API::DX11, LEVEL::SUCCESS);
	}

	void RemoveModel(int IDX) override {}

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
		Dx11Shader _;
		_.InitShader(path, ScreenWidth, ScreenHeight, 45.0f, 0.1f, 100.0f);

		Dx11PostProccess pass;
		pass.SetShaderID(_.GetShaderId());
		PostProccessPasses.push_back(pass);
	}

	void RemovePostProccessPassAtIdx(int Idx) override
	{
		PostProccessPasses.erase(PostProccessPasses.begin() + Idx);
	}

	uint32_t GetRenderTarget() override
	{
		return reinterpret_cast<uint32_t>(renderTargetTexture);
	}

	std::string GetModelsName()
	{
		std::string name;
		for (auto& model : models)
		{
			name += model.GetModelName() + "\n";
		}
		return name;
	}

	int GetSceneVerticesCount()
	{
		int _ = 0;
		for (auto& model : models)
		{
			_ += model.GetVerticesCount();
		}
		return _;
	}

	int GetSceneTriangleCount()
	{
		int _ = 0;
		for (auto& model : models)
		{
			_ += model.GetInfo()->TriangleCount;
		}
		return _;
	}

	void Draw() override
	{
		for (auto& model : models)
		{
			model.Draw();
		}
	}
};
