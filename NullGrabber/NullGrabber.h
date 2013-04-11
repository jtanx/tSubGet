#include "NullGrabberInterfaces.h"
#include "NullGrabberC.h"

//Interface description
MIDL_INTERFACE("08478C61-D22B-416b-93B5-3379CC027A79")
INullGrabberCB : public IUnknown
{
	public:
		virtual STDMETHODIMP SampleCB(
			IMediaSample *pSample,
			REFERENCE_TIME *StartTime,
			REFERENCE_TIME *EndTime) = 0;
};

MIDL_INTERFACE("96287A17-9D0A-4bcc-AD30-1E44A0ABE9D1")
INullGrabber : public IUnknown
{
    public:
		virtual STDMETHODIMP SetAcceptedMediaType( 
			const CMediaType *pType) = 0;
	    
		virtual STDMETHODIMP GetConnectedMediaType( 
			CMediaType *pType) = 0;
	    
		virtual STDMETHODIMP SetCallback( 
			INullGrabberCB *Callback) = 0;
};

class CNullGrabber : public CBaseRenderer,
					 public INullGrabber
{
	public:
		static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
		HRESULT CheckMediaType(const CMediaType *pmt);
		HRESULT DoRenderSample(IMediaSample *pMediaSample);
		
		//(Dont use DECLARE_IUNKNOWN so can override Release)
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		//Expose INullGrabber. 
		STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

		//INullGrabber
		STDMETHODIMP SetAcceptedMediaType(const CMediaType *pmt);
		STDMETHODIMP GetConnectedMediaType(CMediaType *pmt);
		STDMETHODIMP SetCallback(INullGrabberCB *Callback);
	private:
		CCritSec m_Lock; // serialize access to our data
		CMediaType m_mtAccept;
		INullGrabberCB *m_callback;

		CNullGrabber(LPUNKNOWN pUnk, HRESULT *phr);
};

class NGC_Interface : INullGrabberCB 
{
	public:
		NGC_Interface(wchar_t *filepath, const CMediaType *pmtAccept);
		~NGC_Interface();
		void SetSampleCallback(NGC_Callback callback, void *userData);
		void SetProgressCallback(NGC_Progress progress, void *userData);
		HRESULT Run();
		void Abort();

		//INullGrabberCB implementations
		STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
		ULONG STDMETHODCALLTYPE AddRef();
		ULONG STDMETHODCALLTYPE Release();
		STDMETHODIMP SampleCB(IMediaSample *pSample,
			REFERENCE_TIME *StartTime, REFERENCE_TIME *EndTime);
	private:
		//Graph data
		IGraphBuilder *pGraph;
		IMediaControl *pControl;
		IMediaEvent *pEvent;
		IMediaSeeking *pStatus; 
		IBaseFilter *pSourceF;
		CNullGrabber *pNullGrabber;
		__int64 duration;

		//Callback data
		HRESULT hrCallback; //Return value from callback.
		HANDLE abortEvent, runEvent;
		NGC_Callback callback;
		NGC_Progress progress;
		void *callbackUserData, *progressUserData;
		

		//Synchro shit
		CCritSec m_Lock;

		HRESULT initGraphFilters(wchar_t *filepath, const CMediaType *pmtAccept);
		bool findPin(IEnumPins *pEnum, IPin **pin, int direction);
		HRESULT connectFilters();
		void closeGraph();
};
