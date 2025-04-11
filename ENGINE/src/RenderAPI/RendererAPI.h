#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>
#include "SceneAPI/SceneAPI.h"
#include "UIAPI/UIAPI.h"

class RendererAPI
{
public:
	virtual ~RendererAPI() = default;

	virtual void Init() = 0;

	virtual void AddScene() = 0;

	virtual void RemoveScene(int IDX) = 0;

	virtual Scene* GetCurrentScene() = 0;

	virtual void Shutdown() = 0;

	virtual void LoadShader(const char path[]) = 0;

	virtual void SetFinnalPassShader(const char path[]) = 0;

	virtual uint32_t GetDefaultShaderId() = 0;

	virtual void BindDefaultShader() = 0;

	virtual void BindShaderByIdx(int IDX) = 0;

	virtual std::vector<uint32_t> GetShadersIds() = 0;

	virtual uint32_t GetShaderIdByIdx(int Idx) = 0;

	virtual bool createWindow(int width, int height, const char* name) = 0;

	virtual void Clear(float r, float g, float b, float a) = 0;

	virtual void DrawIndexed(uint32_t indexCount) = 0;

	virtual void SetViewport(int x, int y, int width, int height) = 0;

	virtual bool isWindowOpened() = 0;

	virtual void Present() = 0;

	virtual bool InitUI() = 0;

	virtual void DrawUI() = 0;

	virtual void ClearUI() = 0;
};
