#pragma once
#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>

class Shader
{
public:
	virtual ~Shader() = default;

	void SetDxDeviceContext(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon);
	ID3D10Blob* GetVSBuffer();
	virtual void InitShader(const char path[], int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float min_distance = 0.1f, float max_distance = 100.0f, bool fullscreen = false) = 0;
	virtual void UpdateProjection(int SCREEN_WIDTH, int SCREEN_HEIGHT, float FOV, float min_distance = 0.1f, float max_distance = 100.0f) = 0;
	virtual void Bind() {}
	virtual void Unbind() {}
	virtual uint32_t GetShaderId() = 0;
	virtual uint32_t GetUniformLocation(const std::string& name) = 0;
	virtual void SetUniformMatrix4(const std::string& name, const float* matrix) = 0;
	virtual void SetUniformFloat(const std::string& name, float value) = 0;
	virtual void SetUniformVec3(const std::string& name, const float* vector) = 0;
};
