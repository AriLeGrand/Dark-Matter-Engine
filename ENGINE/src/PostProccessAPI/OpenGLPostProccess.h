#pragma once
#include "PostProccessAPI.h"

class OpenGLPostProccess : public PostProccessAPI
{
private:
	uint32_t Shader_Id;
	uint32_t Src_Texture;
	uint32_t Src_Depth;
	uint32_t Src_Normal;

	uint32_t Tar_Texture;
	uint32_t Tar_Depth;
	uint32_t Tar_Normal;

	bool Enabled = true;

public:
	void SetShaderID(uint32_t Id) override
	{
		Shader_Id = Id;
	}

	uint32_t GetShaderID() override
	{
		return Shader_Id;
	}

	void SetRenderSource(uint32_t Source_Texture, uint32_t Source_Depth = -1, uint32_t Source_Normal = -1) override {}

	void SetRenderTarget(uint32_t Target_Texture, uint32_t Target_Depth = -1, uint32_t Target_Normal = -1) override {}

	void Render() override
	{
		if (Enabled == true)
		{
			return;
		}
	}
};
