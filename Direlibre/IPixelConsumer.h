#pragma once

namespace Direlibre
{
	struct IPixelConsumer
	{
		virtual void ConsumePixels(
			_In_ const D2D1_RECT_U &rect,
			_In_bytecount_c_(dataLength) const BYTE *data,
			_In_ size_t dataLength) = 0;

		virtual void Destroy() = 0;
	};
}