#pragma once
#include <windows.ui.xaml.media.dxinterop.h> 

using namespace Microsoft::WRL;
using namespace Windows::Foundation::Metadata;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace Direlibre
{
	[WebHostHidden]
	public ref class Rendering sealed
	{
	private:
		SwapChainPanel^ const m_panel;
		ISwapChainPanelNative *m_panelNative;
		ID2D1Factory2 *m_d2dFactory;
		double
			m_width,
			m_height,
			m_targetWidth,
			m_targetHeight;
		float
			m_compositionScaleX,
			m_compositionScaleY;

		ComPtr<ID3D11Device1>			m_d3dDevice;
		ComPtr<ID3D11DeviceContext1>	m_d3dContext;
		ComPtr<IDXGISwapChain2>         m_swapChain;
		ComPtr<ID2D1Device>				m_d2dDevice;
		ComPtr<ID2D1DeviceContext>		m_d2dContext;
		ComPtr<ID2D1Bitmap1>			m_d2dTargetBitmap;
		ComPtr<ID2D1Bitmap>				m_d2dMemBitmap;

		void OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e);
		void OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ e);
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateSizeDependentResources();
		void OnDeviceLost();
		void Present();
		void Fill();

	public:
		Rendering(SwapChainPanel^ panel);
		virtual ~Rendering();

		void FillRectangle();
	};
}
