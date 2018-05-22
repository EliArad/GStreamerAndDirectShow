#pragma once

#define LIVE_FILTER_NAME TEXT("UDP Push source")
#define LIVE_OUTPIN_NAME L"Out"
 
// {9E9646C2-2C55-494a-91D2-AAFF31C20530}
static const GUID IID_ILiveSource =
{ 0x9e9646c2, 0x2c55, 0x494a, { 0x91, 0xd2, 0xaa, 0xff, 0x31, 0xc2, 0x5, 0x30 } };

 

// {15CF9D07-729A-4ca4-9DAA-95123FBA0EF2}
static const GUID CLSID_CLiveSource =
{ 0x15cf9d07, 0x729a, 0x4ca4, { 0x9d, 0xaa, 0x95, 0x12, 0x3f, 0xba, 0xe, 0xf2 } };



 

// {B936A94D-6A4D-4a21-A763-E9DD8D58FEC8}
static const GUID IID_IUDPPushSource =
{ 0xb936a94d, 0x6a4d, 0x4a21, { 0xa7, 0x63, 0xe9, 0xdd, 0x8d, 0x58, 0xfe, 0xc8 } };


DECLARE_INTERFACE_(ILiveSource, IUnknown)
{
	// Adds bitmap to the video sequence
	STDMETHOD(AddFrame)(HBITMAP hBmp) PURE;

	// Adds pixel data buffer to the video sequence
	STDMETHOD(AddFrame)(BYTE* pBuffer, DWORD size) PURE;

	// Set the video frame info.
	// Default value is width = 704, height = 576 (4CIF) and 32 bits per pixel
	STDMETHOD(SetBitmapInfo)(BITMAPINFOHEADER& bInfo) PURE;

	// Set the expected frame rate of the video.
	// Value should be in range of [0,30]
	// Default value is 0
	STDMETHOD(SetFrameRate)(int frameRate) PURE;
};



DECLARE_INTERFACE_(IUDPPushSource, IUnknown)
{
	STDMETHOD(StartRecording)(WCHAR *FileName) PURE;
	STDMETHOD(StopRecording)() PURE;
	STDMETHOD(SetIpAddress)(WCHAR *IpAddress) PURE;
	STDMETHOD(SetVideoResolution)(int width, int height) PURE;

};
