tSubGet is built using Microsoft Visual C++ 2008 Express,
which is available free of charge.

To build tSubGet (more specifically NullGrabber), 
you will also need:
* DirectX 9.0 SDK Update (February 2005) Extras:
  --> http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=12518
  --> Compile the 'baseclasses' project within this SDK,
      and update the path to the generated library
      correspondingly in the NullGrabber project.
* Resource Hacker to add 'threadingModel="apartment"' to the comclass def for
  the NullGrabber manifest