#pragma once

#include <windows.h>

namespace hmk
{
	class Window
	{
	public:
		Window();
		~Window();

		bool Init(int _width, int _height);
		bool Run();
		LRESULT CALLBACK MemberWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	private:
		HWND m_hWnd;
	};
}