/* An example header file required for an implementation of INullGrabberCB.
   In this case, 'NGCallback' is the implementation of INullGrabberCB.
   The VTable entry must be first in the struct, but I think you can put
   anything after that for your own use.
*/
typedef struct ParentStruct {
	int someData;
	void otherData;
	NGCallback ngCallback;
} ParentStruct;


typedef struct NGCallback {
	const INullGrabberCBVtbl *lpVtbl;
	ParentStruct *pParent;
} NGCallback;