tSubGet is built using Microsoft Visual C++ 2008 Express,
which is available free of charge.

To build tSubGet (more specifically NullGrabber), 
you will also need:
* DirectShow baseclasses library:
  --> Install *from* the Windows SDK: 
      http://www.microsoft.com/en-us/download/details.aspx?id=8279
  --> Ensure you select the 'Samples' option when installing! The other
      options are up to you - you don't have to install the other SDK 
      components, just uncheck them if you don't want them.
  --> Compile the baseclasses project and update the path to the generated
      library correspondingly in the NullGrabber project.
  --> A typical path to this baseclasses project might be:
      C:\Program Files\Microsoft SDKs\Windows\v7.1\Samples\multimedia\directshow\baseclasses

About:
NullGrabber is really just a glorified sample grabber. The only difference
between it and the ISampleGrabber sample filter is that it allows _any_
stream type to be grabbed. It's also implemented as a renderer.