#pragma once

#include "IPixelConsumer.h"

namespace Direlibre
{
	struct IPixelSource
	{
		virtual void Retain() = 0;
		virtual void Release() = 0;

		virtual void Start(IPixelConsumer *pixelConsumer) = 0;
		virtual void Stop() = 0;
	};
}