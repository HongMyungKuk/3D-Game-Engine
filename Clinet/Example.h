#pragma once

#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include "Rasterization.h"

namespace hmk
{
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
	};

	class Example
	{
	public:
		Example();
		Example(HWND hWnd, int width, int height);
		~Example();

		bool Init(HWND hWnd, int width, int height);
		void Update();
		void Render();
		void Clean();

		IDXGISwapChain* GetSwapChain()
		{
			return swapChain;
		}

	private:
		std::vector<glm::vec4> pixels;
		Rasterization rasterization;

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		IDXGISwapChain* swapChain;
		D3D11_VIEWPORT viewport;
		ID3D11RenderTargetView* renderTargetView;
		ID3D11SamplerState* colorSampler;
		ID3D11Texture2D* canvasTexture;
		ID3D11ShaderResourceView* canvasTextureView;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;

		UINT indexCount;
	};
}

