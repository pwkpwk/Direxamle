#include "pch.h"
#include "Rendering.h"

using namespace Windows::Foundation;
using namespace DX;

namespace Direlibre
{
	const float DIPS_PER_INCH  = 96.0f;

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	static const D3D_FEATURE_LEVEL D3D_FEATURE_LEVELS[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	struct BGRA
	{
		BYTE	blue,
				green,
				red,
				alpha;
	};

	//
	// https://code.msdn.microsoft.com/windowsapps/XAML-SwapChainPanel-00cb688b/sourcecode?fileId=99187&pathId=40359581
	//
	Rendering::Rendering(SwapChainPanel^ panel)
	:	m_panel(panel),
		m_panelNative(nullptr),
		m_d2dFactory(nullptr),
		m_width(panel->ActualWidth),
		m_height(panel->ActualHeight),
		m_compositionScaleX(panel->CompositionScaleX),
		m_compositionScaleY(panel->CompositionScaleY),
		m_targetHeight(0),
		m_targetWidth(0)
	{
		LPUNKNOWN punk = reinterpret_cast<LPUNKNOWN>(panel);
		punk->QueryInterface(&m_panelNative);
		m_panel->SizeChanged += ref new ::SizeChangedEventHandler(this, &Rendering::OnSizeChanged);
		m_panel->CompositionScaleChanged += ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &Rendering::OnCompositionScaleChanged);
		CreateDeviceIndependentResources();
		CreateDeviceResources();
		CreateSizeDependentResources();
		Fill();
	}

	Rendering::~Rendering()
	{
		if(nullptr != m_panelNative)
			m_panelNative->Release();
		if (nullptr != m_d2dFactory)
			m_d2dFactory->Release();
	}

	void Rendering::Fill()
	{
		D2D1_RECT_U rect;

		rect.left = 0;
		rect.top = 0;
		rect.right = static_cast<UINT>(m_targetWidth);
		rect.bottom = static_cast<UINT>(m_targetHeight);

		size_t	count	= rect.right * rect.bottom;
		LPBYTE	data	= (LPBYTE)calloc(count, 4);
		UINT32	pitch	= 4 * rect.right;
		BGRA	*bgra	= reinterpret_cast<BGRA*>(data);
		

		for (size_t i = 0; i < count; ++i)
		{
			bgra->blue = 0xFF;
			bgra->green = 0x2F;
			bgra->red = i & 0x3F;
			++bgra;
		}

		m_d2dTargetBitmap->CopyFromMemory(&rect, data, pitch);
		free(data);

		D2D1_RECT_F rectDest = { 60.0F, 60.0F, m_targetWidth - 60.0F, m_targetHeight - 60.0F };
		m_d2dContext->BeginDraw();
		m_d2dContext->DrawBitmap(m_d2dMemBitmap.Get(), rectDest);
		m_d2dContext->EndDraw();

		Present();
	}

	void Rendering::FillRectangle()
	{
		D2D1_RECT_U rect[2];

		rect[0].left = 16;
		rect[0].top = 16;
		rect[0].right = static_cast<UINT>(m_targetWidth) / 4;
		rect[0].bottom = static_cast<UINT>(m_targetHeight) / 4;

		rect[1].left = rect[0].right;
		rect[1].top = rect[0].bottom;
		rect[1].right = static_cast<UINT>(m_targetWidth) / 2;
		rect[1].bottom = static_cast<UINT>(m_targetHeight) / 2;

		if (rect[0].right > rect[0].left && rect[0].bottom > rect[0].top)
		{

			size_t	count = rect[0].right * rect[0].bottom;
			LPBYTE	data = (LPBYTE)calloc(count, 4);
			UINT32	pitch = sizeof(BGRA) * rect[0].right;
			BGRA	*bgra = reinterpret_cast<BGRA*>(data);

			for (size_t i = 0; i < count; ++i)
			{
				bgra->blue = i & 0x2F;
				bgra->green = ( i + 7 ) & 0x1F;
				bgra->red = 0xFF;
				//bgra->alpha = 0xEF;
				++bgra;
			}

			m_d2dTargetBitmap->CopyFromMemory(rect, data, pitch);
			free(data);

			count = (rect[1].right - rect[1].left) * (rect[1].bottom - rect[1].top);
			data = (LPBYTE)calloc(count, 4);
			pitch = sizeof(BGRA) * ( rect[1].right - rect[1].left );
			bgra = reinterpret_cast<BGRA*>(data);

			for (size_t i = 0; i < count; ++i)
			{
				bgra->blue = i & 0x2F;
				bgra->green = 0xEF;
				bgra->red = 0xEF;
				//bgra->alpha = 0xEF;
				++bgra;
			}

			m_d2dTargetBitmap->CopyFromMemory(rect + 1, data, pitch);
			free(data);

			DXGI_PRESENT_PARAMETERS parameters = { 0 };
			RECT rc[2];

			rc[0].left = rect[0].left;
			rc[0].top = rect[0].top;
			rc[0].right = rect[0].right;
			rc[0].bottom = rect[0].bottom;

			rc[1].left = rect[1].left;
			rc[1].top = rect[1].top;
			rc[1].right = rect[1].right;
			rc[1].bottom = rect[1].bottom;

			parameters.DirtyRectsCount = ARRAYSIZE(rc);
			parameters.pDirtyRects = rc;
			parameters.pScrollRect = nullptr;
			parameters.pScrollOffset = nullptr;

			HRESULT hr = S_OK;

			hr = m_swapChain->Present1(1, 0, &parameters);

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				OnDeviceLost();
			}
			else
			{
				ThrowIfFailed(hr);
			}
		}
	}

	void Rendering::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
	{
		if (e->NewSize != e->PreviousSize)
		{
			m_height = e->NewSize.Height < 1.0f ? 1.0f : e->NewSize.Height;
			m_width = e->NewSize.Width < 1.0f ? 1.0f : e->NewSize.Width;

			CreateSizeDependentResources();
			Fill();
		}
	}

	void Rendering::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ e)
	{
		if (m_compositionScaleX != sender->CompositionScaleX || m_compositionScaleY != sender->CompositionScaleY)
		{
			m_compositionScaleX = sender->CompositionScaleX;
			m_compositionScaleY = sender->CompositionScaleY;

			CreateSizeDependentResources();
			Fill();
		}
	}

	void Rendering::CreateDeviceIndependentResources()
	{
		D2D1_FACTORY_OPTIONS options;
		ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if 0
#if defined(_DEBUG) 
		// Enable D2D debugging via SDK Layers when in debug mode. 
		options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif 
#endif

		::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(m_d2dFactory), &options, (void**)&m_d2dFactory);
	}

	void Rendering::CreateDeviceResources()
	{
		// This flag adds support for surfaces with a different color channel ordering than the API default.
		// It is recommended usage, and is required for compatibility with Direct2D.
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if 0
#if defined(_DEBUG)
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

		// Create the DX11 API device object, and get a corresponding context.
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		HRESULT hr;

		hr = ::D3D11CreateDevice(
				nullptr,                    // Specify null to use the default adapter.
				D3D_DRIVER_TYPE_HARDWARE,
				0,
				creationFlags,              // Optionally set debug and Direct2D compatibility flags.
				D3D_FEATURE_LEVELS,              // List of feature levels this app can support.
				ARRAYSIZE(D3D_FEATURE_LEVELS),
				D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
				&device,                    // Returns the Direct3D device created.
				NULL,                       // Returns feature level of device created.
				&context                    // Returns the device immediate context.
			);

		if (FAILED(hr))
		{
			hr = ::D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_WARP, 0, creationFlags, D3D_FEATURE_LEVELS,
				ARRAYSIZE(D3D_FEATURE_LEVELS), D3D11_SDK_VERSION, &device, NULL, &context );
		}
		ThrowIfFailed(hr);

		// Get D3D11.1 device
		ThrowIfFailed( device.As(&m_d3dDevice) );

		// Get D3D11.1 context
		ThrowIfFailed( context.As(&m_d3dContext) );

		// Get underlying DXGI device of D3D device
		ComPtr<IDXGIDevice> dxgiDevice;
		ThrowIfFailed( m_d3dDevice.As(&dxgiDevice) );

		// Get D2D device
		ThrowIfFailed( m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice) );

		// Get D2D context
		ThrowIfFailed( m_d2dDevice->CreateDeviceContext( D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext ) );

#if 0
		// Set D2D text anti-alias mode to Grayscale to ensure proper rendering of text on intermediate surfaces.
		m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
#endif
	}

	void Rendering::CreateSizeDependentResources()
	{
		m_targetWidth = m_width * m_compositionScaleX;
		m_targetHeight = m_height * m_compositionScaleY;
		//
		// Ensure dependent objects have been released.
		//
		m_d2dContext->SetTarget(nullptr);
		m_d2dTargetBitmap = nullptr;
		m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);
		m_d3dContext->Flush();

		// If the swap chain already exists, then resize it.
		if (nullptr != m_swapChain)
		{
			HRESULT hr = m_swapChain->ResizeBuffers( 2, static_cast<UINT>(m_targetWidth), static_cast<UINT>(m_targetHeight),
				DXGI_FORMAT_B8G8R8A8_UNORM, 0 );

			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				// If the device was removed for any reason, a new device and swap chain will need to be created.
				OnDeviceLost();
				return;

			}
			else
			{
				ThrowIfFailed(hr);
			}
		}
		else // Otherwise, create a new one.
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
			swapChainDesc.Width = static_cast<UINT>(m_targetWidth);      // Match the size of the panel.
			swapChainDesc.Height = static_cast<UINT>(m_targetHeight);
			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                  // This is the most common swap chain format.
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1;                                 // Don't use multi-sampling.
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;                                      // Use double buffering to enable flip.
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;        // All Windows Store apps must use this SwapEffect.
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

			ComPtr<IDXGIDevice1> dxgiDevice;
			ComPtr<IDXGIAdapter> dxgiAdapter;
			ComPtr<IDXGIFactory2> dxgiFactory;
			ComPtr<IDXGISwapChain1> swapChain;

			// Get underlying DXGI Device from D3D Device.
			ThrowIfFailed( m_d3dDevice.As(&dxgiDevice) );

			// Get adapter.
			ThrowIfFailed( dxgiDevice->GetAdapter(&dxgiAdapter) );

			// Get factory.
			ThrowIfFailed( dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)) );

			// Create swap chain.
			ThrowIfFailed( dxgiFactory->CreateSwapChainForComposition( m_d3dDevice.Get(), &swapChainDesc, nullptr, &swapChain ) );
			swapChain.As(&m_swapChain);

			// Ensure that DXGI does not queue more than one frame at a time. This both reduces 
			// latency and ensures that the application will only render after each VSync, minimizing 
			// power consumption.
			ThrowIfFailed( dxgiDevice->SetMaximumFrameLatency(1) );
			//
			// Associate swap chain with SwapChainPanel.  This must be done on the UI thread.
			//
			ThrowIfFailed( m_panelNative->SetSwapChain(m_swapChain.Get()) );
		}

		// Ensure the physical pixel size of the swap chain takes into account both the XAML SwapChainPanel's logical layout size and 
		// any cumulative composition scale applied due to zooming, render transforms, or the system's current scaling plateau.
		// For example, if a 100x100 SwapChainPanel has a cumulative 2x scale transform applied, we instead create a 200x200 swap chain 
		// to avoid artifacts from scaling it up by 2x, then apply an inverse 1/2x transform to the swap chain to cancel out the 2x transform.
		DXGI_MATRIX_3X2_F inverseScale = { 0 };
		inverseScale._11 = 1.0f / m_compositionScaleX;
		inverseScale._22 = 1.0f / m_compositionScaleY;

		m_swapChain->SetMatrixTransform(&inverseScale);

		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE/*D2D1_ALPHA_MODE_PREMULTIPLIED*/),
				DIPS_PER_INCH * m_compositionScaleX,
				DIPS_PER_INCH * m_compositionScaleY
				);

		// Direct2D needs the DXGI version of the backbuffer surface pointer.
		ComPtr<IDXGISurface> dxgiBackBuffer;
		DX::ThrowIfFailed( m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)) );

		// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
		ThrowIfFailed( m_d2dContext->CreateBitmapFromDxgiSurface( dxgiBackBuffer.Get(), &bitmapProperties, &m_d2dTargetBitmap ) );
		//
		// Create a 320x240 in-memory bitmap that will be drawn with scaling in the swap chain buffer. Fill the bitmap with purple dots.
		//
		D2D1_SIZE_U size = { 320, 240 };
		D2D1_RECT_U rect = { 0, 0, size.width, size.height };
		D2D1_BITMAP_PROPERTIES memBitmapProperties =
			D2D1::BitmapProperties( D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), DIPS_PER_INCH, DIPS_PER_INCH );

		m_d2dContext->CreateBitmap(size, memBitmapProperties, &m_d2dMemBitmap);
		BGRA *data = reinterpret_cast<BGRA*>(calloc(320 * 240, sizeof(BGRA)));
		for (size_t i = 0; i < 320 * 240; i += 7)
		{
			data[i].red = 0xFF;
			data[i].blue = 0xFF;
			data[i].green = 0x1F;
		}
		m_d2dMemBitmap->CopyFromMemory(&rect, data, 320 * sizeof(BGRA));
		free(data);

		m_d2dContext->SetDpi(DIPS_PER_INCH * m_compositionScaleX, DIPS_PER_INCH * m_compositionScaleY);
		m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
	}

	void Rendering::OnDeviceLost()
	{
		m_swapChain.Reset();

		// Make sure the rendering state has been released.
		m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);

		m_d2dContext->SetTarget(nullptr);
		m_d2dTargetBitmap.Reset();
		m_d2dContext.Reset();
		m_d2dDevice.Reset();
		m_d3dContext->Flush();

		CreateDeviceResources();
		CreateSizeDependentResources();
	}

	void Rendering::Present()
	{
		DXGI_PRESENT_PARAMETERS parameters = { 0 };

		HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

		switch (hr)
		{
		case DXGI_ERROR_DEVICE_REMOVED:
		case DXGI_ERROR_DEVICE_RESET:
			OnDeviceLost();
			break;

		default:
			ThrowIfFailed(hr);
		}
	}
}
