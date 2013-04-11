NullGrabber is a DirectShow filter that acts as a sample grabber for ANY kind
of stream. It is implemented as a renderer-type filter. 

There are three different ways to interface with NullGrabber:
1. Register the NullGrabber dll and interact through COM
2. Use registration free COM (similar to 1.)
3. Use the bundled C api

Case 1:
* Run 'regsvr32 NullGrabber.dll' to register NullGrabber
* Create an instance of it using 
  (C) CoCreateInstance(&CLSID_NullGrabber, NULL, CLSCTX_INPROC_SERVER, &IID_IBaseFilter, &pNullGrabber);
  (C++) CoCreateInstance(CLSID_NullGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**) &pNullGrabber);
* Implement ISampleGrabberCB 
* Use it and NUllGrabber in a DirectShow graph to extract samples

Case 2:
* Instead of registering the DLL, add NullGrabber to the project's
  additional manifest dependencies, e.g:
  type='win32'  name='NullGrabber' version='0.8.5.0' processorArchitecture='X86'
* Continue as per case 1.

As NullGrabber is only a DirectShow filter you will have to construct a
DirectShow graph as necessary yourself to grab samples. If this is not 
desirable, consider case 3 instead.

Case 3:
* Use the C api as defined in NullGrabberC.h to grab samples.
* The C api wraps the DirectShow code needed to grab samples, so if you're 
  looking for an example on what DirectShow code is needed for cases 1 & 2,
  look at its implementation in NullGrabber (see NullGrabberC.cpp)


Other notes:
For an example of how to implement INullGrabberCB in C, see the examples folder.

To keep Regfree com working, ensure that the assembly identity for the
NullGrabber project is set to something like:
NullGrabber, processorArchitecture=X86, version=0.8.5.0, type=win32

Ensure the version number matches between NullGrabber's assembly identity and
the version number used in the manifest dependency of the project it's used in.

Ensure the threading model is kept to 'Apartment' for NullGrabber to work.
This is ensured by the manifest fragment NullGrabber.manifest.