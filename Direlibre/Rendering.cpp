#include "pch.h"
#include "Rendering.h"

using namespace Windows::Foundation;

namespace Direlibre
{
	Rendering::Rendering(SwapChainPanel^ panel)
	:	m_panel(panel),
		m_panelNative(nullptr),
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
	}

	Rendering::~Rendering()
	{
		if(nullptr != m_panelNative)
			m_panelNative->Release();
	}

	void Rendering::OnSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
	{
		m_height = e->NewSize.Height < 1.0f ? 1.0f : e->NewSize.Height;
		m_width = e->NewSize.Width < 1.0f ? 1.0f : e->NewSize.Width;

		CreateSizeDependentResources();
	}

	void Rendering::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ e)
	{
		m_compositionScaleX = sender->CompositionScaleX;
		m_compositionScaleY = sender->CompositionScaleY;

		CreateSizeDependentResources();
	}

	void Rendering::CreateSizeDependentResources()
	{
		m_targetWidth = m_width * m_compositionScaleX;
		m_targetHeight = m_height * m_compositionScaleY;
	}
}
