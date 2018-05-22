#pragma once

#include <streams.h>
#include "IAddFrame.h"

 

typedef struct SLAFilterParams
{
	char Addr[100];

} SLAFilterParams;

int ReadParamsteres();

class CLiveSourceStream;

class CLiveSource : public CBaseFilter, public IAMFilterMiscFlags
{
public:
	DECLARE_IUNKNOWN;

	CLiveSource(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CLiveSource(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	int GetPinCount();
	CBasePin* GetPin(int n);

	virtual ULONG STDMETHODCALLTYPE GetMiscFlags( void)
	{
		return AM_FILTER_MISC_FLAGS_IS_SOURCE;
	}

	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause();
	STDMETHODIMP Stop();


private:
	CLiveSourceStream* m_pOutputPin;
	CCritSec m_critSec;
};

class CLiveSourceStream : public CBaseOutputPin, public ILiveSource, public IAMPushSource, public IUDPPushSource
{
public:

	DECLARE_IUNKNOWN;

	CLiveSourceStream(CBaseFilter *pFilter, CCritSec *pLock, HRESULT *phr);
	virtual ~CLiveSourceStream();

	int m_videoResolutionWidth;
	int m_videoResolutionHeight;
	int m_videoFrameSize;
	int m_videoSLAFrameSize;

	// CBaseOutputPin overrides
	virtual HRESULT GetMediaType(int iPosition, CMediaType* pmt);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest);

	// ISLA3000 Interface
	virtual STDMETHODIMP StartRecording(WCHAR *FileName);
	virtual STDMETHODIMP StopRecording();
	virtual STDMETHODIMP SetIpAddress(WCHAR *IpAddress);
	virtual STDMETHODIMP SetVideoResolution(int width, int height);


	// ILiveSource members
	virtual STDMETHODIMP AddFrame(HBITMAP hBmp);
	virtual STDMETHODIMP AddFrame(BYTE* pBuffer, DWORD size);
	virtual STDMETHODIMP SetFrameRate(int frameRate);
	virtual STDMETHODIMP SetBitmapInfo(BITMAPINFOHEADER& bInfo);

	// IAMPushSource members
	virtual STDMETHODIMP GetPushSourceFlags(ULONG *pFlags);
	virtual STDMETHODIMP SetPushSourceFlags(ULONG Flags);
	virtual STDMETHODIMP SetStreamOffset(REFERENCE_TIME rtOffset);
	virtual STDMETHODIMP GetStreamOffset(REFERENCE_TIME *prtOffset);
	virtual STDMETHODIMP GetMaxStreamOffset(REFERENCE_TIME *prtMaxOffset);
	virtual STDMETHODIMP SetMaxStreamOffset(REFERENCE_TIME rtMaxOffset);
	virtual STDMETHODIMP GetLatency(REFERENCE_TIME *prtLatency);

	virtual STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

private:
	HRESULT GetPixelData(HBITMAP hBmp, BYTE** ppData, int* pSize);
	HRESULT GetMediaSample(IMediaSample** ppSample);

private:
	BITMAPINFOHEADER m_bmpInfo;
	int m_frameRate;
	REFERENCE_TIME m_rtFrameRate; 
	REFERENCE_TIME m_lastFrame; 
};
