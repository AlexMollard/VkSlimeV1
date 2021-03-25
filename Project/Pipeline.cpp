#include "Pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

Pipeline::Pipeline(const std::string& vertFilePath, const std::string& fragFilePath)
{
	CreateGraphicsPipeLine(vertFilePath, fragFilePath);
}

std::vector<char> Pipeline::readFile(const std::string& filePath)
{
	std::ifstream file{ filePath, std::ios::ate | std::ios::binary };

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

void Pipeline::CreateGraphicsPipeLine(const std::string& vertFilePath, const std::string& fragFilePath)
{
	auto vertCode = readFile(vertFilePath);
	auto fragCode = readFile(fragFilePath);

	std::cout << "Vertex shader size: " << vertCode.size() << std::endl;
	std::cout << "Fragment shader size: " << fragCode.size() << std::endl;
}