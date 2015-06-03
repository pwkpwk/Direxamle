#include "pch.h"
#include "Class1.h"

using namespace Direlibre;
using namespace Platform;
using namespace Microsoft::WRL;

Class1::Class1()
{
}

void Class1::Test12()
{
	ComPtr<ID3D12Device> device;

	HRESULT hr = ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0,
		__uuidof(ID3D12Device), (void**)(device.ReleaseAndGetAddressOf()));

	if (SUCCEEDED(hr))
	{
		::OutputDebugString(TEXT("SUCCEEDED D3D12CreateDevice\n"));
	}
	else
	{
		::OutputDebugString(TEXT("FAILED D3D12CreateDevice\n"));
	}
}

void Class1::Test11()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = ::D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,					// Returns the Direct3D device created.
		&featureLevel,			// Returns feature level of device created.
		&context					// Returns the device immediate context.
		);

	if (SUCCEEDED(hr))
	{
		::OutputDebugString(TEXT("SUCCEEDED D3D11CreateDevice\n"));
	}
	else
	{
		::OutputDebugString(TEXT("FAILED D3D11CreateDevice\n"));
	}
}
