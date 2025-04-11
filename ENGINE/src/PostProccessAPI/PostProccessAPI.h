#pragma once

class PostProccessAPI
{
public:
	virtual ~PostProccessAPI() = default;

	virtual void SetShaderID(uint32_t Id) = 0;

	virtual uint32_t GetShaderID() = 0;

	virtual void SetRenderSource(uint32_t Source_Texture, uint32_t Source_Depth, uint32_t Source_Normal) = 0;

	virtual void SetRenderTarget(uint32_t Target_Texture, uint32_t Target_Depth, uint32_t Target_Normal) = 0;

	virtual void Render() = 0;
};
