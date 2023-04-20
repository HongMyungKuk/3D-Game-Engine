#include "Example.h"

namespace hmk
{
	Example::Example()
	{
	}

	Example::Example(HWND hWnd, int width, int height)
		: rasterization()
	{
		Init(hWnd, width, height);
	}

	Example::~Example()
	{
	}

	bool Example::Init(HWND hWnd, int width, int height)
	{
		//DXGI_FORMAT_R32G32B32_FLOAT // RED, GREEN, BLUE 32bit float로 구성
		//DXGI_FORMAT_R16G16B16A16_UNORM // RED, GREEN, BLUE, ALPHA 16bit로 0~1사이 정규화(normalized) 된 값으로 구성
		//DXGI_FORMAT_R32G32_UINT // red, green 32bit 정수로 구성
		//DXGI_FORMAT_R8G8B8A8_UNORM // red,green,blue,alpha 8 bit로 0~1값을 가짐
		//DXGI_FORMAT_R8G8B8A8_SNORM // red,green,blue,alpha 8 bit로 -1~1 값을 가짐
		//DXGI_FORMAT_R8G8B8A8_SINT // red,green,blue,alpha 8 bit로 -128~127 값을 가짐
		//DXGI_FORMAT_R8G8B8A8_UINT // red,green,blue,alpha 8 bit로 0~255 값을 가짐
		//https://zenoahn.tistory.com/69

		DXGI_SWAP_CHAIN_DESC sc;
		ZeroMemory(&sc, sizeof(sc));
		sc.BufferDesc.Width = width;							// set the back buffer width
		sc.BufferDesc.Height = height;							// set the back buffer height
		sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// red,green,blue,alpha 8 bit로 0~1값을 가짐
		sc.BufferCount = 2;										// one back buffer
		sc.BufferDesc.RefreshRate.Numerator = 60;
		sc.BufferDesc.RefreshRate.Denominator = 1;
		sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// how swap chain is to be used
		sc.OutputWindow = hWnd;
		sc.SampleDesc.Count = 1;
		sc.Windowed = TRUE;										// windowed//full-screen
		sc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// allow full-screen
		sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		const D3D_FEATURE_LEVEL featureLevelArray[] = { D3D_FEATURE_LEVEL_11_0 };
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
			createDeviceFlags, featureLevelArray, 1, D3D11_SDK_VERSION, &sc, &swapChain,
			&device, NULL, &deviceContext)))
		{
			std::cout << "D3D11CreateDeviceAndSwapChain() error" << std::endl;
		}
		
		// CreateRenderTarget
		ID3D11Texture2D* pBackBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		if (pBackBuffer)
		{
			device->CreateRenderTargetView(pBackBuffer, NULL, &renderTargetView);
			pBackBuffer->Release();
		}
		else
		{
			std::cout << "CreateRenderTargetView() error" << std::endl;
			exit(-1);
		}

		// Set the viewport
		ZeroMemory(&viewport, sizeof(viewport));
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = float(width);
		viewport.Height = float(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deviceContext->RSSetViewports(1, &viewport);

		// Create texture
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&sd, &colorSampler);

		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Width = width;
		td.Height = height;
		td.MipLevels = td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_DYNAMIC;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		device->CreateTexture2D(&td, NULL, &canvasTexture);

		if (canvasTexture)
		{
			device->CreateShaderResourceView(canvasTexture, NULL, &canvasTextureView);
		}
		else
		{
			std::cout << "CreateRenderTargetView() error" << std::endl;
		}
		
		{
			const std::vector<Vertex> vertices = {
				{
					{-1.0f, -1.0f, 0.0f, 1.0f},
					{0.f, 1.f},
				},
				{
					{1.0f, -1.0f, 0.0f, 1.0f},
					{1.f, 1.f},
				},
				{
					{1.0f, 1.0f, 0.0f, 1.0f},
					{1.f, 0.f},
				},
				{
					{-1.0f, 1.0f, 0.0f, 1.0f},
					{0.f, 0.f},
				},
			};

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // write access access by
			// CPU and GPU
			bufferDesc.ByteWidth = UINT(
				sizeof(Vertex) * vertices.size()); // size is the VERTEX struct * 3
			bufferDesc.BindFlags =
				D3D11_BIND_VERTEX_BUFFER; // use as a vertex buffer
			bufferDesc.CPUAccessFlags =
				D3D11_CPU_ACCESS_WRITE; // allow CPU to write
			// in buffer
			bufferDesc.StructureByteStride = sizeof(Vertex);

			D3D11_SUBRESOURCE_DATA vertexBufferData = {
				0,
			};
			vertexBufferData.pSysMem = vertices.data();
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr =
				device->CreateBuffer(&bufferDesc, &vertexBufferData, &vertexBuffer);
			if (FAILED(hr)) {
				std::cout << "CreateBuffer() failed. " << std::hex << hr
					<< std::endl;
			};
		}

		// Create an index buffer
		{
			const std::vector<uint16_t> indices = {
				3, 1, 0, 2, 1, 3,
			};

			indexCount = UINT(indices.size());

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // write access access by
			// CPU and GPU
			bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
			bufferDesc.BindFlags =
				D3D11_BIND_INDEX_BUFFER; // use as a vertex buffer
			bufferDesc.CPUAccessFlags =
				D3D11_CPU_ACCESS_WRITE; // allow CPU to write
			// in buffer
			bufferDesc.StructureByteStride = sizeof(uint16_t);

			D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
			indexBufferData.pSysMem = indices.data();
			indexBufferData.SysMemPitch = 0;
			indexBufferData.SysMemSlicePitch = 0;

			device->CreateBuffer(&bufferDesc, &indexBufferData, &indexBuffer);
		}

		ID3DBlob* vertexBlob = nullptr;
		ID3DBlob* pixelBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		HRESULT hr;

		hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0,
			&vertexBlob, &errorBlob);
		if (FAILED(hr)) {
			// 파일이 없을 경우
			if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
				std::cout << "File not found." << std::endl;
			}

			// 에러 메시지가 있으면 출력
			if (errorBlob) {
				std::cout << "Vertex shader compile error\n"
					<< (char*)errorBlob->GetBufferPointer() << std::endl;
			}

			exit(-1); // 강제 종료
		}

		hr = D3DCompileFromFile(L"PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0,
			&pixelBlob, &errorBlob);
		if (FAILED(hr)) {
			// 파일이 없을 경우
			if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0) {
				std::cout << "File not found." << std::endl;
			}

			// 에러 메시지가 있으면 출력
			if (errorBlob) {
				std::cout << "Pixel shader compile error\n"
					<< (char*)errorBlob->GetBufferPointer() << std::endl;
			}

			exit(-1); // 강제 종료
		}

		device->CreateVertexShader(vertexBlob->GetBufferPointer(),
			vertexBlob->GetBufferSize(), NULL,
			&vertexShader);
		device->CreatePixelShader(pixelBlob->GetBufferPointer(),
			pixelBlob->GetBufferSize(), NULL, &pixelShader);

		// Create the input layout object
		D3D11_INPUT_ELEMENT_DESC ied[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16,
			 D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		device->CreateInputLayout(ied, 2, vertexBlob->GetBufferPointer(),
			vertexBlob->GetBufferSize(), &layout);

		pixels.resize(width * height);

		return true;
	}

	void Example::Update()
	{
		std::fill(pixels.begin(), pixels.end(),
			glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)); // 검은 배경

		// 렌더링
		// rasterization.Render(pixels);

		D3D11_MAPPED_SUBRESOURCE ms;
		deviceContext->Map(canvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, pixels.data(), pixels.size() * sizeof(glm::vec4));
		deviceContext->Unmap(canvasTexture, NULL);
	}

	void Example::Render()
	{
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		deviceContext->RSSetViewports(1, &viewport);
		deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

		// set the shader objects
		deviceContext->VSSetShader(vertexShader, 0, 0);
		deviceContext->PSSetShader(pixelShader, 0, 0);

		// select which vertex buffer to display
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		deviceContext->IASetInputLayout(layout);
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// https://github.com/Microsoft/DirectXTK/wiki/Getting-Started
		// https://github.com/microsoft/Xbox-ATG-Samples/tree/main/PCSamples/IntroGraphics/SimpleTexturePC
		deviceContext->PSSetSamplers(0, 1,
			&colorSampler); // TODO: samplers to array
		deviceContext->PSSetShaderResources(0, 1, &canvasTextureView);
		deviceContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->DrawIndexed(indexCount, 0, 0);
	}

	void Example::Clean()
	{
		if (layout) {
			layout->Release();
			layout = NULL;
		}
		if (vertexShader) {
			vertexShader->Release();
			vertexShader = NULL;
		}
		if (pixelShader) {
			pixelShader->Release();
			pixelShader = NULL;
		}
		if (vertexBuffer) {
			vertexBuffer->Release();
			vertexBuffer = NULL;
		}
		if (indexBuffer) {
			indexBuffer->Release();
			indexBuffer = NULL;
		}
		if (canvasTexture) {
			canvasTexture->Release();
			canvasTexture = NULL;
		}
		if (canvasTextureView) {
			canvasTextureView->Release();
			canvasTextureView = NULL;
		}
		if (colorSampler) {
			colorSampler->Release();
			colorSampler = NULL;
		}
		if (renderTargetView) {
			renderTargetView->Release();
			renderTargetView = NULL;
		}
		if (swapChain) {
			swapChain->Release();
			swapChain = NULL;
		}
		if (deviceContext) {
			deviceContext->Release();
			deviceContext = NULL;
		}
		if (device) {
			device->Release();
			device = NULL;
		}
	}
}