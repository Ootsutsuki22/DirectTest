#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

// Show Supported Fearure Level in Window Caption
void ShowFeatureLevel(HWND hwnd, D3D_FEATURE_LEVEL level)
{
	switch (level)
	{
	case D3D_FEATURE_LEVEL_1_0_CORE:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_1_0_CORE");
		break;
	case D3D_FEATURE_LEVEL_9_1:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_9_1");
		break;
	case D3D_FEATURE_LEVEL_9_2:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_9_2");
		break;
	case D3D_FEATURE_LEVEL_9_3:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_9_3");
		break;
	case D3D_FEATURE_LEVEL_10_0:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_10_0");
		break;
	case D3D_FEATURE_LEVEL_10_1:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_10_1");
		break;
	case D3D_FEATURE_LEVEL_11_0:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_11_0");
		break;
	case D3D_FEATURE_LEVEL_11_1:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_11_1");
		break;
	case D3D_FEATURE_LEVEL_12_0:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_12_0");
		break;
	case D3D_FEATURE_LEVEL_12_1:
		SetWindowTextW(hwnd, L"D3D_FEATURE_LEVEL_12_1");
		break;
	default:
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	// Create Window

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"MyWindowClassName";
	wcex.hIconSm = nullptr;
	if (!RegisterClassExW(&wcex))
	{
		MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
		return GetLastError();
	}

	RECT initialRect = { 0, 0, 1280, 720 };
	AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	LONG initialWidth = initialRect.right - initialRect.left;
	LONG initialHeight = initialRect.bottom - initialRect.top;

	HWND hwnd = CreateWindowExW(0,
		wcex.lpszClassName,
		L"My Direct3D 11 Application",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		initialWidth,
		initialHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!hwnd)
	{
		MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
		return GetLastError();
	}
	ShowWindow(hwnd, nCmdShow);

	// Create D3D Device

	ID3D11Device* baseDevice = nullptr;
	ID3D11DeviceContext* baseDeviceContext = nullptr;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	//Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		Flags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&baseDevice,
		nullptr,
		&baseDeviceContext
		);

	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDevice(nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			Flags,
			&featureLevels[1],
			ARRAYSIZE(featureLevels) - 1,
			D3D11_SDK_VERSION,
			&baseDevice,
			nullptr,
			&baseDeviceContext
		);
	}

	if (FAILED(hr))
	{
		MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);
		return GetLastError();
	}

	ShowFeatureLevel(hwnd, baseDevice->GetFeatureLevel());

	IDXGISwapChain* d3d11SwapChain = nullptr;
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = baseDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr))
		{
			IDXGIAdapter* dxgiAdapter = nullptr;
			hr = dxgiDevice->GetAdapter(&dxgiAdapter);
			if (SUCCEEDED(hr))
			{
				/*DXGI_ADAPTER_DESC adapterDesc;
				dxgiAdapter->GetDesc(&adapterDesc);
				SetWindowTextW(hwnd, adapterDesc.Description);*/
				hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				dxgiAdapter->Release();
			}

			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
	{
		MessageBoxA(0, "Obtain DXGI factory from device failed", "Fatal Error", MB_OK);
		return GetLastError();
	}

	DXGI_SWAP_CHAIN_DESC sd = {};	
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;	
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	//sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; /* SurfaceFormat.Color */
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 3;	
	sd.OutputWindow = hwnd;	
	sd.Windowed = 1;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	hr = dxgiFactory->CreateSwapChain(baseDevice, &sd, &d3d11SwapChain);
	if (FAILED(hr))
	{
		MessageBoxA(0, "SwapChain creation failed", "Fatal Error", MB_OK);
		return GetLastError();
	}
	dxgiFactory->Release();

	// Create Framebuffer Render Target
	ID3D11RenderTargetView* d3d11FrameBufferView;
	{
		ID3D11Texture2D* d3d11FrameBuffer;
		HRESULT hResult = d3d11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);

		hResult = baseDevice->CreateRenderTargetView(d3d11FrameBuffer, 0, &d3d11FrameBufferView);
		d3d11FrameBuffer->Release();
	}

	bool isRunning = true;
	MSG msg = {};
	while (isRunning)
	{
		while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				isRunning = false;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
		baseDeviceContext->ClearRenderTargetView(d3d11FrameBufferView, backgroundColor);

		d3d11SwapChain->Present(1, 0);
	}

	return 0;
}


