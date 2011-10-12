#include <streams.h>
#include <initguid.h>
#include "NullGrabber.h"

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
	HRESULT hr = S_OK;
	
	if(m_callback)
	{
		REFERENCE_TIME StartTime, StopTime;
		
		pMediaSample->GetTime(&StartTime, &StopTime);
		StartTime += m_pInputPin->CurrentStartTime();
		StopTime  += m_pInputPin->CurrentStartTime();
		hr = m_callback( pMediaSample, &StartTime, &StopTime);
	}

	return hr;
}