#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FreeImage.h"
#include "ModelAPI/ModelAPI.h"
#include "ModelAPI/ModelInfo.h"
#include "ModelAPI/ModelLoader.h"
#include "ShaderAPI/OpenGLShader.h"

#include "UIAPI/UIAPI.h"

class OpenGLModel : public Model
{
private:
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
	GLuint EBO;

	GLuint texture_id;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<unsigned int> indices;
	std::vector<float> colors;
	std::vector<unsigned int> texcoord_indices;
	std::vector<unsigned int> normal_indices;

	std::vector<float> vertex_data;

	Model_loader ModelLoaderInterface;
	std::vector<float> ModelData; // x, y, z, nx, ny, nz, u, v
	//OpenGLShader m_Program;

	ModelInfo Info;

	GLuint load_texture(const std::string& filepath)
	{
		// Ensure FreeImage is initialized before use
		//if (!FreeImage_IsInitialized()) {
		FreeImage_Initialise();
		//}

		// Detect the file format
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filepath.c_str(), 0);
		if (format == FIF_UNKNOWN)
		{
			std::cerr << "Failed to determine file format: " << filepath << std::endl;
			FreeImage_DeInitialise();
			return 0;
		}

		// Load the image
		FIBITMAP* image = FreeImage_Load(format, filepath.c_str());
		if (!image)
		{
			std::cerr << "Failed to load image: " << filepath << std::endl;
			FreeImage_DeInitialise();
			return 0;
		}

		// Convert to 32-bit (RGBA)
		FIBITMAP* image32 = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image); // Unload the original image to save memory
		if (!image32)
		{
			std::cerr << "Failed to convert image to 32 bits: " << filepath << std::endl;
			FreeImage_DeInitialise();
			return 0;
		}

		// Get image dimensions and data
		int width = FreeImage_GetWidth(image32);
		int height = FreeImage_GetHeight(image32);
		unsigned char* data = FreeImage_GetBits(image32);

		// Generate and bind OpenGL texture
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload texture to GPU
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

		// Free FreeImage resources
		FreeImage_Unload(image32);
		FreeImage_DeInitialise();

		return texture_id;
	}

	bool LoadModel(std::string filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cerr << "Failed to open the file." << std::endl;
			return false;
		}
		else
		{
			std::cout << "Model: [" << filename << "] found..." << std::endl;
		}

		std::string line;
		while (std::getline(file, line))
		{
			if (line.substr(0, 2) == "v ")
			{
				std::istringstream s(line.substr(2));
				float x, y, z;
				s >> x >> y >> z;
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			}

			if (line.substr(0, 2) == "vt")
			{
				std::istringstream s(line.substr(2));
				float u, v;
				s >> u >> v; // Only read the first two values
				texcoords.push_back(u);
				texcoords.push_back(v);
			}

			if (line.substr(0, 2) == "vn")
			{
				std::istringstream s(line.substr(2));
				float x, y, z;
				s >> x >> y >> z;
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
			}

			if (line.substr(0, 2) == "f ")
			{
				std::istringstream s(line.substr(2));
				std::string vertex_info;

				while (s >> vertex_info)
				{
					std::istringstream vertex_stream(vertex_info);
					std::string v, vt, vn;

					// Parse vertex/texture/normal indices
					std::getline(vertex_stream, v, '/'); // Vertex index
					std::getline(vertex_stream, vt, '/'); // Texture index
					std::getline(vertex_stream, vn, '/'); // Normal index (optional)

					// Store vertex index (convert to 0-based)
					indices.push_back(std::stoi(v) - 1);

					// Store texture index (convert to 0-based, only if present)
					if (!vt.empty())
					{
						texcoord_indices.push_back(std::stoi(vt) - 1);
					}

					// Store normal index (convert to 0-based, only if present)
					if (!vn.empty())
					{
						normal_indices.push_back(std::stoi(vn) - 1);
					}
				}
			}
		}

		//for (size_t i = 0; i < indices.size(); i += 3) {
		//    std::swap(indices[i], indices[i + 2]);  // Swap first and last index
		//}

		file.close();
		return true;
	}

	void process()
	{
		// Ensure there are valid indices and the corresponding arrays
		if (indices.empty() || vertices.empty())
		{
			std::cerr << "Error: Indices or vertices are empty!" << std::endl;
			return;
		}

		for (size_t i = 0; i < indices.size(); i++)
		{
			int v_idx = indices[i] * 3; // Multiply by 3 for x, y, z
			vertex_data.push_back(vertices[v_idx]);
			vertex_data.push_back(vertices[v_idx + 1]);
			vertex_data.push_back(vertices[v_idx + 2]);

			if (!texcoords.empty())
			{
				int vt_idx = texcoord_indices[i] * 2;
				vertex_data.push_back(texcoords[vt_idx]);
				vertex_data.push_back(texcoords[vt_idx + 1]);
			}

			if (!normals.empty())
			{
				int vn_idx = normal_indices[i] * 3;
				vertex_data.push_back(normals[vn_idx]);
				vertex_data.push_back(normals[vn_idx + 1]);
				vertex_data.push_back(normals[vn_idx + 2]);
			}
		}
	}

public:
	OpenGLModel() {}
	~OpenGLModel() {}

	void UpdateModelInfo()
	{
		Info.VAO = VAO;
		Info.VBO = VBO;
		Info.TriangleCount = vertices.size();
		Info.vertexs = vertices;
		Info.indicies = indices;
	}

	bool Load(const char path[], const char texture_path[], uint32_t ShaderProgramId) override
	{
		LoadModel(path);
		process();

		texture_id = load_texture(texture_path);
		ModelLoaderInterface.filename = path;

		// Generate and bind the VAO (Vertex Array Object)
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Generate and bind the VBO (Vertex Buffer Object)
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

		// Generate and bind the IBO (Index Buffer Object)
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		// Compute stride based on available attributes (position, UVs, normals)
		int stride = 3 * sizeof(float); // Position is always 3 floats
		if (!texcoords.empty())
			stride += 2 * sizeof(float); // UV coordinates
		if (!normals.empty())
			stride += 3 * sizeof(float); // Normals

		// Define vertex attribute layout
		int offset = 0;

		// Position attribute (always present)
		glEnableVertexAttribArray(0); // Location 0 in shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(std::intptr_t)offset);
		offset += 3 * sizeof(float);

		// Texture coordinates attribute (if available)
		if (!texcoords.empty())
		{
			glEnableVertexAttribArray(1); // Location 1 in shader
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(std::intptr_t)offset);
			offset += 2 * sizeof(float);
		}

		// Normals attribute (if available)
		if (!normals.empty())
		{
			glEnableVertexAttribArray(2); // Location 2 in shader
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(std::intptr_t)offset);
		}

		// Unbind VAO (good practice to avoid accidental modifications)
		glBindVertexArray(0);

		UpdateModelInfo();
		Info.ShaderProgramId = ShaderProgramId;
		std::cout << "Done loading asset..." << std::endl;
		return true;
	}

	void Deload() override {}

	void Draw() override
	{
		//m_Program.Bind();
		glUseProgram(Info.ShaderProgramId);
		//std::cout << "Binded: " << Info.ShaderProgramId << std::endl;
		//exit(0);

		glm::vec3 cameraPosition = {0.0f, 1.5f, 5.0f};
		glm::quat cameraRotation = glm::quat(glm::radians(glm::vec3(-25, 0, 0)));

		glm::vec3 position = glm::make_vec3(Info.position);

		//glm::vec3 eulerAngles = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = glm::quat(glm::radians(glm::make_vec3(Info.rotation)));
		glm::vec3 scale = {1.0f, 1.0f, 1.0f};

		//float size = 1.0f;
		//float x_pos = -0.240f;
		//float y_pos = -0.800f;
		//float z_pos = 2.960f;
		glm::vec3 offset_position = {0.0f, 0.0f, 0.0f};
		glm::quat offset_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 offset_scale = {1.0f, 1.0f, 1.0f};

		//GLint modelLoc = m_Program.GetUniformLocation("model");
		//GLint viewLoc = m_Program.GetUniformLocation("view");

		GLint modelLoc = glGetUniformLocation(Info.ShaderProgramId, "model");
		GLint viewLoc = glGetUniformLocation(Info.ShaderProgramId, "view");

		// Set up camera properties (position, rotation)
		glm::mat4 view_matrix = glm::mat4(1.0f); // Identity matrix
		glm::vec3 cameraFront = glm::normalize(cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 cameraUp = glm::normalize(cameraRotation * glm::vec3(0.0f, 1.0f, 0.0f));

		// Camera position and view matrix
		view_matrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		// Model matrix (for object transformation)
		glm::mat4 model_matrix = glm::mat4(1.0f); // Identity matrix (no transformation)

		// Apply translation (position)
		model_matrix = glm::translate(model_matrix, position);

		// Apply rotation using quaternion
		model_matrix *= glm::mat4_cast(rotation);

		// Apply scaling
		model_matrix = glm::scale(model_matrix, scale);

		//std::vector<glm::vec3> animation_transform_values = animation.get_current_values();
		// std::cout << "Done." << animation_transform_values[1].x << std::endl;

		// Apply offset transformations (position, rotation, scaling)
		model_matrix = glm::translate(model_matrix, offset_position);
		//model_matrix = glm::translate(model_matrix, animation_transform_values[0]);
		model_matrix *= glm::mat4_cast(offset_rotation); // Apply rotation
		//model_matrix *= glm::mat4_cast(glm::quat(glm::radians(animation_transform_values[1])));  // Apply rotation
		model_matrix = glm::scale(model_matrix, offset_scale); // Apply scaling
		//model_matrix = glm::scale(model_matrix, animation_transform_values[2]);  // Apply scaling

		// Set the final model matrix to the shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));

		glEnable(GL_DEPTH_TEST);
		// Enable face culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back faces (default)
		glFrontFace(GL_CCW); // Counterclockwise winding order

		glBindVertexArray(VAO);

		// Bind VBO before setting attributes
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, texture_id);

		int stride = 3 * sizeof(float); // Position toujours 3 floats
		if (!texcoords.empty())
			stride += 2 * sizeof(float); // UVs
		if (!normals.empty())
			stride += 3 * sizeof(float); // Normales

		int offset = 0;

		// Activation et configuration des positions (location 0)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(std::intptr_t)offset);
		offset += 3 * sizeof(float);

		// Activation et configuration des UVs (location 1)
		if (!texcoords.empty())
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(std::intptr_t)offset);
			offset += 2 * sizeof(float);
		}

		// Activation et configuration des normales (location 2)
		if (!normals.empty())
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(std::intptr_t)offset);
		}

		GLenum error = glGetError();
		if (glGetError() != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error: " << error << std::endl;
		}

		//std::cout << "Drawing: " << indices.size() << std::endl;

		//glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)indices.size());
		////glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

		// Disable the attributes
		glDisableVertexAttribArray(0); // Disable position attribute
		glDisableVertexAttribArray(1); // Disable texture attribute
		glDisableVertexAttribArray(2); // Disable normal attribute

		// Unbind the VAO
		glBindVertexArray(0);
		glUseProgram(0);
		//m_Program.Unbind();
	}

	ModelInfo* GetInfo() override
	{
		return &Info;
	}

	int GetVerticesCount()
	{
		return vertices.size();
	}

	std::string GetModelName()
	{
		return ModelLoaderInterface.filename;
	}

	void SetDxDeviceContext(ID3D11Device* dxDevice, ID3D11DeviceContext* d3d11DevCon, ID3D10Blob* DxVS_Buffer) override {}
};
