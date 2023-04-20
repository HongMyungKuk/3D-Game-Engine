#include "Window.h"
#include "Example.h"

hmk::Window* g_window;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	return g_window->MemberWndProc(hWnd, iMessage, wParam, lParam);
}

namespace hmk
{
	Window::Window()
	{
		g_window = this;
	}

	Window::~Window()
	{
	}

	bool Window::Init(int _width, int _height)
	{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX),
			CS_CLASSDC,
			WndProc,
			0L,
			0L,
			GetModuleHandle(NULL),
			NULL,
			NULL,
			NULL,
			NULL,
			L"Engine",
			NULL };

		RegisterClassEx(&wc);

		RECT wr = { 0, 0, _width, _height };

		// �ʿ��� ������ ũ��(�ػ�) ���
		// wr�� ���� �ٲ�
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		// �����츦 ���鶧 ������ ����� wr ���
		HWND hwnd;
		m_hWnd = hwnd = CreateWindow(wc.lpszClassName, L"Engine",
			WS_OVERLAPPEDWINDOW,
			100, // ������ ���� ����� x ��ǥ
			100, // ������ ���� ����� y ��ǥ
			wr.right - wr.left, // ������ ���� ���� �ػ�
			wr.bottom - wr.top, // ������ ���� ���� �ػ�
			NULL, NULL, wc.hInstance, NULL);

		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);

		return true;
	}

	bool Window::Run()
	{
		MSG msg = {};

		Example* example = new Example(m_hWnd, 1280, 900);

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				example->Update();
				example->Render();

				example->GetSwapChain()->Present(1, 0);
			}
		}
		example->Clean();

		return true;
	}

	LRESULT Window::MemberWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
	{
		HDC hdc;
		PAINTSTRUCT ps;

		switch (iMessage) {
		case WM_CREATE:
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	}
}