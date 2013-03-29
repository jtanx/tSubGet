#include <streams.h>
#include <Windows.h>
#include <stdarg.h>
#include <wchar.h>
#include "NullGrabber.h"
#include "NullGrabberC.h"

#define IsEventActive(hEvent) (WaitForSingleObject((hEvent), 0) == WAIT_OBJECT_0)

static void setSystemError(NGC_ErrorInfo *error, HRESULT hr)
{
	if (error) {
		error->errorCode = NGC_COM_ERROR;
		if (error->bufferSize > 0 && error->buffer) {
			wchar_t buf[MAX_PATH];
			if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
					hr, 0, buf, MAX_PATH, NULL)) 
			{
				_snwprintf_s(error->buffer, error->bufferSize, _TRUNCATE,
					L"Error 0x%08lX: %s", hr, buf);
			}
		}
	}
}

static void setError(NGC_ErrorInfo *error, int code, wchar_t *fmt, ...)
{
	if (error) {
		error->errorCode = code;
		if (error->bufferSize > 0 && error->buffer) {
			va_list args;
			va_start(args, fmt);
			_vsnwprintf_s(error->buffer, error->bufferSize, 
				_TRUNCATE, fmt, args);
			va_end(args);
		}
	}
}

class NGC_Interface : INullGrabberCB {
public:
	NGC_Interface(wchar_t *filepath, const CMediaType *pmtAccept, NGC_ErrorInfo *error) : 
	  pControl(NULL), pEvent(NULL), pGraph(NULL), 
		  pNullGrabber(NULL), pSourceF(NULL), pStatus(NULL),
		  callback(NULL), progress(NULL), userData(NULL)
	{
		HRESULT hr;
		if (!filepath) {
			setError(error, NGC_INVALID_ARGUMENTS, L"No input file specified");
			throw NGC_INVALID_ARGUMENTS;
		}
		
		if (FAILED((hr = initGraphFilters(filepath, pmtAccept)))) {
			setSystemError(error, hr);
			closeGraph();
			throw NGC_COM_ERROR;
		} else if (!connectFilters(error)) {
			closeGraph();
			throw NGC_ERROR;
		}

		//Create the abort event.
		abortEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		runEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	~NGC_Interface() {
		closeGraph();

		CloseHandle(abortEvent);
		CloseHandle(runEvent);
	}

	void SetSampleCallback(NGC_Callback callback, void *userData) {
		CAutoLock lock(&m_Lock);
		this->callback = callback;
		this->userData = userData;
	}

	void SetProgressCallback(NGC_Progress progress) {
		CAutoLock lock(&m_Lock);
		this->progress = progress;
	}

	bool Run() {
		HRESULT hr;
		long state;

		//Prevent multiple calls to run!
		if (IsEventActive(this->runEvent)) {
			return false;
		} 

		SetEvent(this->runEvent);
		this->pControl->Run();
		do{
			if (IsEventActive(this->abortEvent)){
				this->pControl->Stop();
				return false;
			} 

			hr = this->pEvent->WaitForCompletion(100, &state);
			if (SUCCEEDED(hr)){
				if (state == EC_COMPLETE){ 
					//Set the callback/progress to 100%. Signal a finish!
					if (this->progress) {
						this->progress(this->duration, this->duration);
					}
					if (this->callback) {
						//Special signal: NULL buffer/0 bytes long to indicate finish
						this->callback(NULL, 0, duration, this->userData);
					}
					
					//IMPORTANT: Stop graph to prevent memory leaks.
					this->pControl->Stop();
					return true;
				}
				else return false; //User abort?
			}

			this->pControl->GetState(100, &state);
		} while (state == State_Running);

		return false;
	}

	void Abort() {
		SetEvent(abortEvent);
	}

	//INullGrabberCB implementations.
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) {
		return E_NOINTERFACE;
	}
	ULONG STDMETHODCALLTYPE AddRef() {
		return 2;
	}
	ULONG STDMETHODCALLTYPE Release() {
		return 1;
	}
	STDMETHODIMP SampleCB(IMediaSample *pSample,
		REFERENCE_TIME *StartTime, REFERENCE_TIME *EndTime) {
		unsigned char *buffer;
		HRESULT hr = S_OK;
		CAutoLock lock(&m_Lock);
	
		if (this->progress) {
			this->progress(*StartTime, this->duration);
		}

		if (this->callback && !IsEventActive(abortEvent)) {
			hr = pSample->GetPointer(&buffer);
			if (SUCCEEDED(hr) && buffer != NULL) {
				if (!this->callback(buffer, pSample->GetActualDataLength(), 
					*StartTime, this->userData)) 
				{
					SetEvent(abortEvent); //Stop!
					return S_FALSE;
				}
			}
		}
	
		return hr;
	}

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
	HANDLE abortEvent, runEvent;
	NGC_Callback callback;
	NGC_Progress progress;
	void *userData;
	

	//Synchro shit
	CCritSec m_Lock;

	//Graph initialization and creation
	HRESULT initGraphFilters(wchar_t *filepath, const CMediaType *pmtAccept){
		HRESULT hr;
		hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
								IID_IGraphBuilder, (void**) &pGraph);
		if (SUCCEEDED(hr)) {
			IMediaFilter *pMedia;

			pGraph->QueryInterface(IID_IMediaControl, (void**) &pControl);
			pGraph->QueryInterface(IID_IMediaEvent, (void**) &pEvent);
			pGraph->QueryInterface(IID_IMediaSeeking, (void**) &pStatus);
			pGraph->QueryInterface(IID_IMediaFilter, (void**) &pMedia);
			
			//Ensure correct time format.
			pStatus->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);

			//This part is important: It makes DS run as fast as possible, instead of at playback speed.
			pMedia->SetSyncSource(NULL);
			pMedia->Release();

			hr = pGraph->AddSourceFilter(filepath, L"Source", &pSourceF);
			if (FAILED(hr))
				return hr;
			
			//Manual instantiation - correct??
			pNullGrabber = (CNullGrabber*) CNullGrabber::CreateInstance(NULL, &hr);
			pNullGrabber->AddRef();

			hr = pGraph->AddFilter(pNullGrabber, L"Null Grabber");
			if (FAILED(hr))
				return hr;

			pNullGrabber->SetAcceptedMediaType(pmtAccept);
			pNullGrabber->SetCallback(this);
		}

		return hr;
	}

	static bool findPin(IEnumPins *pEnum, IPin **pin, int direction){
		PIN_INFO pinfo;

		*pin = NULL;
		while(pEnum->Next(1, pin, NULL) == S_OK){
			(*pin)->QueryPinInfo(&pinfo);
			if (pinfo.pFilter)
				pinfo.pFilter->Release();
			
			if(pinfo.dir == direction)
				break;
			(*pin)->Release();
			*pin = NULL;
		}

		return (*pin) != 0;
	}

	bool connectFilters(NGC_ErrorInfo *error){
		HRESULT hr;
		IEnumPins *pEnum;
		IPin *pinIn, *pinOut;

		//Get the input pin of the null grabber filter
		pNullGrabber->EnumPins(&pEnum);
		pEnum->Reset();
		if (!findPin(pEnum, &pinIn, PINDIR_INPUT)) {
			setError(error, NGC_COM_ERROR, 
				L"Could not find the input pin of the Null Grabber filter");
			return false;
		}
		pEnum->Release();

		//Find the right output pin of the source filter and connect.
		pSourceF->EnumPins(&pEnum);
		pEnum->Reset();
		while (findPin(pEnum, &pinOut, PINDIR_OUTPUT)){
			hr = pGraph->ConnectDirect(pinOut, pinIn, NULL);
			if (hr == S_OK)
				break;
			pinOut->Release();
			pinOut = NULL;
		}
		pEnum->Release();
		pinIn->Release();

		if (!pinOut) {
			setError(error, NGC_NO_INPUT_STREAM,
				L"No connection could be negotiated between the source filter and NullGrabber for the specified media type.");
			return false;
		}

		//Get the duration, now that everything's connected.
		this->pStatus->GetDuration(&duration);
		pinOut->Release();
		return true;
	}

	
#define RELEASE_AND_ZERO(item) if (item) { item->Release(); item = NULL; }
	void closeGraph(){
		RELEASE_AND_ZERO(pNullGrabber);
		RELEASE_AND_ZERO(pSourceF);
		RELEASE_AND_ZERO(pEvent);
		RELEASE_AND_ZERO(pControl);
		RELEASE_AND_ZERO(pStatus);
		RELEASE_AND_ZERO(pGraph);
	}
};

//C interface API stubs
API_DECLARE(NGC_Info *) NGC_Init(wchar_t *filepath, AM_MEDIA_TYPE *pmtAccept, NGC_ErrorInfo *error) {
	NGC_Interface *iface = NULL;

	try {
		iface = new NGC_Interface(filepath, (CMediaType *) pmtAccept, error);
	} catch (NGC_ErrorCodes) {
	}

	return (NGC_Info *) iface;
}

API_DECLARE(void) NGC_SetSampleCallback(NGC_Info *info, NGC_Callback callback, void *userData) {
	if (info) {
		((NGC_Interface *) info)->SetSampleCallback(callback, userData);
	}
}

API_DECLARE(void) NGC_SetProgressCallback(NGC_Info *info, NGC_Progress progress) {
	if (info) {
		((NGC_Interface *) info)->SetProgressCallback(progress);
	}
}

API_DECLARE(int) NGC_Run(NGC_Info *info) {
	if (info) {
		return ((NGC_Interface *)info)->Run();
	}
	return false;
}

API_DECLARE(void) NGC_Abort(NGC_Info *info) {
	if (info) {
		((NGC_Interface *)info)->Abort();
	}
}

API_DECLARE(void) NGC_Free(NGC_Info **info) {
	if (info && *info) {
		delete ((NGC_Interface *) *info);
		*info = NULL;
	}
}