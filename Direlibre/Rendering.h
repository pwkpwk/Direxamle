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
		float
			m_width,
			m_height,
			m_compositionScaleX,
			m_compositionScaleY,
			m_targetWidth,
			m_targetHeight;

		void OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e);
		void OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ e);
		void CreateSizeDependentResources();

	public:
		Rendering(SwapChainPanel^ panel);
		virtual ~Rendering();
	};
}
