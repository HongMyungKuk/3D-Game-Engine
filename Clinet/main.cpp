#include <iostream>
#include "Window.h"

int main()
{
	hmk::Window window;
	const int width = 1280, height = 900;

	if (!window.Init(width, height))
	{
		return -1;
	}
	return window.Run();
}
