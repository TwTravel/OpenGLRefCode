// Common.h
// Common definitions shared among files
// Shared rendering state information

struct RENDER_STATE
	{
	unsigned int uiFrames;	// Running Frame count
	BOOL bResize;			// Flag that window was resized	
	BOOL bTerminate;		// Flag to terminate rendering loop
	BOOL bFatalError;		// Fatal error has occured in render thread
	BOOL bModifyFlag;		// Flag set whenever something changes
	};


// Rendering Thread Entry Point
void RenderingThreadEntryPoint(void *pVoid);
