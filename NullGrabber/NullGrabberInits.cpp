#include "streams.h"
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