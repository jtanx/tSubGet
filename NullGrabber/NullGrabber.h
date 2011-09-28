// {3CFFEB81-A06E-4e1d-AFAB-9E6AF667D087}
DEFINE_GUID(CLSID_NullGrabber, 
0x3cffeb81, 0xa06e, 0x4e1d, 0xaf, 0xab, 0x9e, 0x6a, 0xf6, 0x67, 0xd0, 0x87);

// {96287A17-9D0A-4bcc-AD30-1E44A0ABE9D1}
DEFINE_GUID(IID_INullGrabber, 
0x96287a17, 0x9d0a, 0x4bcc, 0xad, 0x30, 0x1e, 0x44, 0xa0, 0xab, 0xe9, 0xd1);

typedef HRESULT (*NGCALLBACK) (
    IMediaSample * pSample, 
    REFERENCE_TIME * StartTime, 
    REFERENCE_TIME * StopTime );

//Interface description
MIDL_INTERFACE("4549d97d-e8ef-4fb4-896e-2f7f867eca2f")
INullGrabber : public IUnknown
{
    public:
		virtual HRESULT STDMETHODCALLTYPE SetAcceptedMediaType( 
			const CMediaType *pType) = 0;
	    
		virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
			CMediaType *pType) = 0;
	    
		virtual HRESULT STDMETHODCALLTYPE SetCallback( 
			NGCALLBACK Callback) = 0;
};

class CNullGrabber : public CBaseRenderer,
					 public INullGrabber
{
	public:
		static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
		HRESULT CheckMediaType(const CMediaType *pmt);
		HRESULT DoRenderSample(IMediaSample *pMediaSample);
		
		//Expose INullGrabber
		DECLARE_IUNKNOWN;
		STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

		//INullGrabber
		STDMETHODIMP SetAcceptedMediaType(const CMediaType *pmt);
		STDMETHODIMP GetConnectedMediaType(CMediaType *pmt);
		STDMETHODIMP SetCallback(NGCALLBACK Callback);
	private:
		CCritSec m_Lock; // serialize access to our data
		CMediaType m_mtAccept;
		NGCALLBACK m_callback;

		CNullGrabber(LPUNKNOWN pUnk, HRESULT *phr);
};
