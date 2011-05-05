#include <XnCppWrapper.h>
#include "XnVNite.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// This macro checks the return code that all OpenNI calls make
// and throws an error if the return code is an error. Use this
// after all calls to OpenNI objects. Great for debugging.
//
// This macro is copied from here:
// http://www.codingbeta.com/?p=24
//
#define CHECK_RC(rc, what)											\
if (rc != XN_STATUS_OK)											\
{																\
printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
return rc;													\
}

bool run = true;

int main(int argc, char* argv[])
{
    XnStatus nRetVal = XN_STATUS_OK;
    
    xn::Context context;
    
    // Initialize context object
    nRetVal = context.Init();
    CHECK_RC(nRetVal, "Initialize context");
    
    // Create a Hands tracker node
    xn::HandsGenerator hands;
    nRetVal = hands.Create(context);
    CHECK_RC(nRetVal, "Initialize Hands node");
    
    // Start generating data
    nRetVal = context.StartGeneratingAll();
    CHECK_RC(nRetVal, "Start generating data");
    
    
    while(run) {
        // Wait for new data
        nRetVal = context.WaitOneUpdateAll(hands);
        CHECK_RC(nRetVal, "Wait for new data");
        
        
    }
    
    
    
	
	return 0;
}