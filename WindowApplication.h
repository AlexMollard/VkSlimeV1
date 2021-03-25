#pragma once
#include "Device.h"
#include "Window.h"
#include "Pipeline.h"

class WindowApplication
{
public:
	static constexpr int WIDTH = 1920;
	static constexpr int HEIGHT = 1080;

	void run();
private:
	Window window{WIDTH, HEIGHT, "VSLIME BBY!"};
	Device device{ window };
	Pipeline pipeline{device, "Shaders/SimpleShader.vert.spv", "Shaders/SimpleShader.frag.spv", Pipeline::DefaultPipelineConfigInfo(WIDTH, HEIGHT) };
};