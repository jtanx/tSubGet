#include <streams.h>
#include <initguid.h>
#include "NullGrabber.h"

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_NULL,            // Major type
    &MEDIASUBTYPE_NULL          // Minor type
};

const AMOVIESETUP_PIN sudPins =
{
    L"Input",                   // Pin string name
    FALSE,                      // Is it rendered
    FALSE,                      // Is it an output
    FALSE,                      // Allowed none
    FALSE,                      // Likewise many
    &CLSID_NULL,                // Connects to filter
    L"Output",                  // Connects to pin
    1,                          // Number of types
    &sudPinTypes                // Pin information
};

const AMOVIESETUP_FILTER sudNullGrabber =
{
    &CLSID_NullGrabber,			// Filter CLSID
    L"Null Grabber",			// String name
    MERIT_DO_NOT_USE,			// Filter merit
    1,							// Number pins
    &sudPins					// Pin details
};

// Needed for the CreateInstance mechanism
CFactoryTemplate g_Templates[]=
{
    { L"Null Grabber"
        , &CLSID_NullGrabber
        , CNullGrabber::CreateInstance
        , NULL
        , &sudNullGrabber }

};

int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

STDAPI DllRegisterServer() 
{
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer() 
{
    return AMovieDllRegisterServer2(FALSE);
}

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

CUnknown * WINAPI CNullGrabber::CreateInstance(LPUNKNOWN punk, HRESULT *phr) 
{
    ASSERT(phr);
    CNullGrabber *pNewObject = new CNullGrabber(punk, phr);
    if(pNewObject == NULL) {
        if (phr)
            *phr = E_OUTOFMEMORY;
    }
    return pNewObject;   

} 

CNullGrabber::CNullGrabber(LPUNKNOWN pUnk, HRESULT *phr):
CBaseRenderer(CLSID_NullGrabber, NAME("Null Grabber"), pUnk, phr){
	m_callback = NULL;
	m_mtAccept = CMediaType();
}

STDMETHODIMP CNullGrabber::NonDelegatingQueryInterface(REFIID riid, void ** ppv) 
{
    CheckPointer(ppv,E_POINTER);
	
    if(riid == IID_INullGrabber) {                
        return GetInterface((INullGrabber *) this, ppv);
    }
    else {
        return CBaseRenderer::NonDelegatingQueryInterface(riid, ppv);
    }
}

STDMETHODIMP CNullGrabber::SetAcceptedMediaType(const CMediaType *pmt)
{
	HRESULT hr;
	CAutoLock lock( &m_Lock );

	if(!pmt){
		m_mtAccept = CMediaType();
		return S_OK;
	}

	hr = CopyMediaType(&m_mtAccept, pmt);
	return hr;	
}

STDMETHODIMP CNullGrabber::GetConnectedMediaType(CMediaType *pmt)
{
	if(!m_pInputPin || !m_pInputPin->IsConnected()){
		return VFW_E_NOT_CONNECTED;
	}

	return m_pInputPin->ConnectionMediaType(pmt);
}

STDMETHODIMP CNullGrabber::SetCallback(NGCALLBACK Callback )
{
    CAutoLock lock( &m_Lock );

    m_callback = Callback;
    return S_OK;
}

HRESULT CNullGrabber::CheckMediaType(const CMediaType *pmt){
	GUID checkGUID;
	CheckPointer(pmt,E_POINTER);
	CAutoLock lock( &m_Lock );

	checkGUID = *(m_mtAccept.Type());
	if (IsEqualGUID(checkGUID,GUID_NULL))
		return S_OK;
	else if (!IsEqualGUID(checkGUID,*(pmt->Type())))
		return VFW_E_INVALID_MEDIA_TYPE;

	checkGUID = *(m_mtAccept.Subtype());
	if (IsEqualGUID(checkGUID,GUID_NULL))
		return S_OK;
	else if (!IsEqualGUID(checkGUID,*(pmt->Type())))
		return VFW_E_INVALID_MEDIA_TYPE;
	
	checkGUID = *(m_mtAccept.FormatType());
	if (IsEqualGUID(checkGUID,GUID_NULL))
		return S_OK;
	else if (!IsEqualGUID(checkGUID,*(pmt->FormatType())))
		return VFW_E_INVALID_MEDIA_TYPE;

	return S_OK;
}

HRESULT CNullGrabber::DoRenderSample(IMediaSample *pMediaSample){
	CheckPointer(pMediaSample, E_POINTER);
	CAutoLock lock( &m_Lock );
	
	if(m_callback)
	{
		HRESULT hr;
		REFERENCE_TIME StartTime, StopTime;
		
		pMediaSample->GetTime(&StartTime, &StopTime);
		StartTime += m_pInputPin->CurrentStartTime();
		StopTime  += m_pInputPin->CurrentStartTime();

		pMediaSample->AddRef();
		hr = m_callback( pMediaSample, &StartTime, &StopTime);
		pMediaSample->Release();

		return hr;
	}
	return S_OK;
}