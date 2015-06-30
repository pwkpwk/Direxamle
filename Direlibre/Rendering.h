#pragma once
#include <windows.ui.xaml.media.dxinterop.h> 
#include "IPixelSource.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation::Metadata;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace Direlibre
{
	struct BGRA
	{
		BYTE	blue,
				green,
				red,
				alpha;
	};

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

		class PixelConsumer : IPixelConsumer
		{
		private:
			Rendering ^m_rendering;

		public:
			static IPixelConsumer *Create(_In_ Rendering^ rendering)
			{
				return new PixelConsumer(rendering);
			}

		private:
			PixelConsumer(_In_ Rendering^ rendering) : m_rendering(rendering)
			{
			}

			~PixelConsumer()
			{
				m_rendering = nullptr;
			}

			void ConsumePixels(
				_In_ const D2D1_RECT_U &rect,
				_In_bytecount_c_(dataLength) const BYTE *data,
				_In_ size_t dataLength) override
			{
				if (rect.right > rect.left && rect.bottom > rect.top)
				{
					size_t pitch = (rect.right - rect.left) * sizeof(BGRA);

					if (dataLength == pitch * (rect.bottom - rect.top))
					{
						m_rendering->m_d2dMemBitmap->CopyFromMemory(&rect, data, static_cast<UINT32>(pitch));

						D2D1_RECT_F rectDest =
						{
							60.0F,
							60.0F,
							static_cast<FLOAT>(m_rendering->m_targetWidth - 60.0),
							static_cast<FLOAT>(m_rendering->m_targetHeight - 60.0)
						};

						m_rendering->m_d2dContext->BeginDraw();
						m_rendering->m_d2dContext->DrawBitmap(m_rendering->m_d2dMemBitmap.Get(), rectDest);
						m_rendering->m_d2dContext->EndDraw();
						m_rendering->Present();
					}
				}
			}

			void Destroy() override
			{
				delete this;
			}
		};

		IPixelSource *m_pixelSource;

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

		void AttachPixelSource(::Platform::IntPtr source);

		void FillRectangle();
	};
}
