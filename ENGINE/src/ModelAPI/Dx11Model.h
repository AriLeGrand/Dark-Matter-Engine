#pragma once
#include "ModelAPI.h"
#include "ModelLoader.h"
#include "ShaderAPI/Dx11Shader.h"
#include "ModelInfo.h"
#include "ConsoleManager.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>
#include <vector>

class Dx11Model : public Model
{
private:
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11Device* device;

	Model_loader ModelLoaderInterface;

	std::vector<float> ModelData; // x, y, z, nx, ny, nz, u, v
	//Dx11Shader m_Program;

	ID3D10Blob* errorBuffer;
	ID3D10Blob* VS_Buffer;
	ModelInfo Info;
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC layout[3] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Position (x, y, z, w)
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Normal (nx, ny, nz)
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, // UV (u, v)
	};

	UINT numElements = ARRAYSIZE(layout);
	ID3D11InputLayout* vertLayout = nullptr;

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<unsigned int> indices;
	std::vector<float> colors;
	std::vector<uint32_t> texcoord_indices;
	std::vector<uint32_t> normal_indices;

	std::vector<float> vertex_data;

	bool LoadModel(std::string filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			console.Log("Failed to open the file.", API::DX11, LEVEL::ERRORS);
			//std::cerr << "Failed to open the file." << std::endl;
			return false;
		}
		else
		{
			console.Log("Model: [" + filename + "] found...", API::DX11, LEVEL::INFO);
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
			console.Log("Error: Indices or vertices are empty!", API::DX11, LEVEL::ERRORS);
			return;
		}

		for (size_t i = 0; i < indices.size(); i++)
		{
			int v_idx = indices[i] * 3; // Multiply by 3 for x, y, z
			vertex_data.push_back(vertices[v_idx]);
			vertex_data.push_back(vertices[v_idx + 1]);
			vertex_data.push_back(vertices[v_idx + 2]);
			//vertex_data.push_back(1.0f);

			if (!normals.empty())
			{
				int vn_idx = normal_indices[i] * 3;
				vertex_data.push_back(normals[vn_idx]);
				vertex_data.push_back(normals[vn_idx + 1]);
				vertex_data.push_back(normals[vn_idx + 2]);
			}
			if (!texcoords.empty())
			{
				int vt_idx = texcoord_indices[i] * 2;
				vertex_data.push_back(texcoords[vt_idx]);
				vertex_data.push_back(texcoords[vt_idx + 1]);
			}
		}
	}

public:
	Dx11Model() {}
	~Dx11Model() {}

	void UpdateModelInfo()
	{
		// Info.VAO = VAO;
		// Info.VBO = VBO;
		Info.TriangleCount = vertices.size();
		Info.vertexs = vertices;
		Info.indicies = indices;
	}

	void SetDxDeviceContext(ID3D11Device* dxDevice, ID3D11DeviceContext* d3d11DevCon, ID3D10Blob* DxVS_Buffer)
	{
		deviceContext = d3d11DevCon;
		device = dxDevice;
		VS_Buffer = DxVS_Buffer;
	}

	bool Load(const char path[], const char texture_path[], uint32_t ShaderProgramId) override
	{
		// UpdateModelInfo();
		LoadModel(path);
		process();
		UpdateModelInfo();
		Info.ShaderProgramId = ShaderProgramId;
		ModelLoaderInterface.filename = path;
		//std::cout << "Done loading asset..." << std::endl;

		// Définition des données des vertices sous forme de std::vector<float>
		// Format : xyznxnynzuv
		//std::vector<float> vertices = {
		//    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // Vertex 1
		//    -0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // Vertex 2
		//     0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // Vertex 3
		//     0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f   // Vertex 4
		//};

		// Utilisation d'un std::vector<int> pour les indices
		//std::vector<int> indices = {
		//    0, 1, 2,
		//    0, 2, 3
		//};

		for (int i = 0; i < 8; i++)
		{
			std::cout << vertex_data[i] << " ";
		}
		std::cout << std::endl;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * indices.size(); // Taille du vecteur d'indices
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;
		if (!device)
		{
			console.Log("device is null");
		}
		iinitData.pSysMem = indices.data();
		hr = device->CreateBuffer(&indexBufferDesc, &iinitData, &m_IndexBuffer);

		if (FAILED(hr))
		{
			if (errorBuffer)
			{
				console.Log("Vertex buffer creation failed", API::DX11, LEVEL::ERRORS);
				errorBuffer->Release();
			}
			return false;
		}

		// Update primitive topology to triangle list
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Définir la description du buffer des vertices
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(float) * vertex_data.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

		// Pointer vers les donn�es des vertices
		vertexBufferData.pSysMem = vertex_data.data();

		hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);

		if (FAILED(hr))
		{
			if (errorBuffer)
			{
				console.Log("Vertex buffer creation failed: ", API::DX11, LEVEL::ERRORS);
				errorBuffer->Release();
			}
			return false;
		}

		// D�finir les informations sur le buffer de vertices
		UINT stride = sizeof(float) * 8; // 8 valeurs par vertex (xyznxnynzuv)
		UINT offset = 0;

		// IA = Input Assembler
		deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

		// Création de l'input layout
		if (!VS_Buffer)
		{
			console.Log("Vertex Shader buffer is NULL. Shader compilation likely failed.", API::DX11, LEVEL::ERRORS);
			return false;
		}

		hr = device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &vertLayout);
		if (FAILED(hr))
		{
			if (errorBuffer)
			{
				console.Log("Input layout creation failed:" + std::string((const char*)(errorBuffer->GetBufferPointer())), API::DX11, LEVEL::ERRORS);
				errorBuffer->Release();
			}
			return false;
		}

		// Définir le layout
		deviceContext->IASetInputLayout(vertLayout);

		// D�finir la topologie primitive (triangles)
		//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//Create the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = 1280;
		viewport.Height = 720;

		//Set the viewport
		deviceContext->RSSetViewports(1, &viewport);

		console.Log("Loading success", API::DX11, LEVEL::SUCCESS);
		return true;
	}

	void Deload() override {}

	void Draw() override
	{
		//m_Program.Bind();
		//deviceContext->DrawIndexed(indexCount, 0, 0);
		//m_Program.Unbind();
		// 
		//display triangle
		//deviceContext->DrawIndexed(indices.size(), 0, 0);
		deviceContext->Draw(indices.size(), 0);
		//std::cout << "Drawing: " << indices.size() << std::endl;
	}

	ModelInfo* GetInfo() override
	{
		return &Info;
	}

	//std::vector<float> GetVertexs() {return }
	/*btTriangleMesh* GetBulletTriangleMesh() override {}*/

	int GetVerticesCount()
	{
		return vertices.size();
	};

	std::string GetModelName() const
	{
		return ModelLoaderInterface.filename;
	};
};
