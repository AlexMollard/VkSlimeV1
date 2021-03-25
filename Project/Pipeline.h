#pragma once
#include <string>
#include <vector>

class Pipeline
{
public:
	Pipeline(const std::string& vertFilePath, const std::string& fragFilePath);

private:
	static std::vector<char> readFile(const std::string& filePath);

	void CreateGraphicsPipeLine(const std::string& vertFilePath, const std::string& fragFilePath);
};
