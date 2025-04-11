#pragma once
#include <cstdint>
#include <vector>

class ModelInfo
{
public:
	uint32_t ShaderProgramId = 0;
	uint32_t VAO;
	uint32_t VBO;
	int TriangleCount = 0;
	float position[3] = {0.0f, 0.0f, 0.0f};
	float rotation[3] = {0.0f, 0.0f, 0.0f};
	float size[3] = {0.0f, 0.0f, 0.0f};

	std::vector<float> vertexs;
	std::vector<unsigned int> indicies;
};
