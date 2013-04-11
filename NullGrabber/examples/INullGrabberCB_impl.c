/* Example code to implement INullGrabberCB in C.
*/

#include <initguid.h> //DO NOT REMOVE - to initialise the custom GUIDs used by NullGrabber.
#include "..\\NullGrabber\\NullGrabberInterfaces.h"
#include "INullGrabberCB_impl.h"


/* QueryInterface, AddRef and Release are all function stubs used to
   implement INullGrabberCB (pseudo-COM interface)
*/
HRESULT STDMETHODCALLTYPE QueryInterface(INullGrabberCB *this, REFIID riid, void **ppvObject){
	return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE AddRef(INullGrabberCB *this){
	return 2;
}

HRESULT STDMETHODCALLTYPE Release(INullGrabberCB *this){
	return 1;
}

/*
   SampleCB(this, pSample, StartTime, EndTime) is the main callback
   function for samples received from the Null Grabber. 
*/
HRESULT STDMETHODCALLTYPE SampleCB(INullGrabberCB *this, IMediaSample *pSample, REFERENCE_TIME *StartTime, REFERENCE_TIME *EndTime){
	ParentStruct *p = ((NGCallback*) this)->pParent; //Cast it back to our implementation to access custom fields.

	//Do stuff with data using ParentStruct...
	
	return S_OK;
}

/* 
   The VTable needed for this implementation of INullGrabberCB
*/
static const INullGrabberCBVtbl INullGrabberCB_Vtbl = {
	QueryInterface,
	AddRef,
	Release,
	SampleCB
};

/*
   ngCallbackinit(p) 'initialises' INullGrabberCB/NGCallback
   with the required settings, and enables the callback function
   SampleCB to work.
*/
void ngCallbackInit(ParentStruct *p){
	p->NullGrabberCB.lpVtbl = &INullGrabberCB_Vtbl;
	p->NullGrabberCB.pParent = p;
}


/* Example usage:
   ParentStruct p;
   ngCallbackInit(&p);

   ...Directshow code...

   //pNullGrabber is an INullGrabber instance
   pNullGrabber->lpVtbl->SetCallback(pNullGrabber, (INullGrabberCB*) &p->NullGrabberCB);
*/