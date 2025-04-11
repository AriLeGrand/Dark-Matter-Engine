#pragma once
#include "ModelInfo.h"
#include <d3d11.h>

class Model
{
public:
	virtual ~Model() = default;

	virtual bool Load(const char path[], const char texture_path[], uint32_t ShaderProgramId) = 0;
	virtual void Deload() = 0;
	virtual void Draw() = 0;

	/*virtual std::vector<float> GetVertexs() = 0;
	virtual std::vector<float> GetIndices() = 0;*/

	virtual void SetDxDeviceContext(ID3D11Device* dxDevice, ID3D11DeviceContext* d3d11DevCon, ID3D10Blob* DxVS_Buffer) = 0;

	virtual ModelInfo* GetInfo() = 0;

	/*virtual btTriangleMesh* GetBulletTriangleMesh() = 0;*/
};
