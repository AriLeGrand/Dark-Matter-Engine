#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Bullet/BulletCollision/CollisionShapes/btTriangleMesh.h"

class Model_loader
{
private:
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<unsigned int> indices;
	std::vector<float> colors;
	std::vector<unsigned int> texcoord_indices;
	std::vector<unsigned int> normal_indices;

	std::vector<float> vertex_data;

public:
	std::string filename;

	std::vector<float> getVerticesData()
	{
		return vertex_data;
	}

	std::vector<float> getVertices()
	{
		return vertices;
	}

	std::vector<float> getNormals()
	{
		return normals;
	}

	std::vector<float> getTexcoords()
	{
		return texcoords;
	}

	std::vector<unsigned int> getIndices()
	{
		return indices;
	}

	Model_loader() {}

	bool load(std::string file_path_arg)
	{
		filename = file_path_arg;

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
};
