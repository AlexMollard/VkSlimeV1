#include "WindowApplication.h"

void WindowApplication::run()
{
	while (!window.ShouldClose())
	{
		glfwPollEvents();
	}
}