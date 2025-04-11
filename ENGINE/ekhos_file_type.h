#pragma once
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <fstream>


static void extractParts(float A, __int16& OUT1, __int16& OUT2) {
	OUT1 = static_cast<__int16>((double)A);
	OUT2 = static_cast<__int16>(((double)A - OUT1) * 1000);
}


static void save_array(std::vector<float> array) {
	std::ofstream file(std::string("output.dat"), std::ios::out | std::ios::binary); // Open file in binary mode
	if (!file) {
		std::cerr << "Error: Could not open file for writing!" << std::endl;
		return;
	}

	for (int idx = 0; idx < array.size(); idx++) {
		__int16 OUT1, OUT2;
		extractParts(array.at(idx), OUT1, OUT2);
		std::cout << array.at(idx) << std::endl;
	

		// Write data to file
		file.write(reinterpret_cast<const char*>(&OUT1), sizeof(OUT1));
		file.write(reinterpret_cast<const char*>(&OUT2), sizeof(OUT2));
	}

	file.close();
}


static void load_array() {
	std::ifstream file("output.dat", std::ios::in | std::ios::binary);
	if (!file) {
		std::cerr << "Error: Could not open file for reading!" << std::endl;
	}
	else {
		__int16 out1, out2;

		while (file.read(reinterpret_cast<char*>(&out1), sizeof(out1)) &&
			file.read(reinterpret_cast<char*>(&out2), sizeof(out2))) {

			std::cout << "OUT1: " << out1 << ", OUT2: " << (float)out2/1000 << " Finnal: " << out1 + ((float)out2/1000) << std::endl;
		}
		file.close();
	}
}