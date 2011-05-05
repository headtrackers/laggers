#include <XnCppWrapper.h>
#include "XnVNite.h"
#include <iostream>

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

void XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt)
{
	printf("Session progress (%6.2f,%6.2f,%6.2f) - %6.2f [%s]\n", ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z, fProgress,  strFocus);
}

void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& pFocus, void* UserCxt)
{
    std::cout << "Session started" << std::endl;
    
    return;
}
void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
    return;
}

int main(int argc, char* argv[])
{
    XnStatus nRetVal = XN_STATUS_OK;
    
    xn::Context context;
    XnVSessionManager sessionManager;
    
    // Initialize context object
    nRetVal = context.Init();
    CHECK_RC(nRetVal, "Initialize context");
    
    xn::DepthGenerator depth;
    nRetVal = depth.Create(context);
    CHECK_RC(nRetVal, "Initialize Depth node");
    
    // Create a Hands tracker node
    xn::HandsGenerator hands;
    nRetVal = hands.Create(context);
    CHECK_RC(nRetVal, "Initialize Hands node");
    
    // Create a Hands tracker node
    xn::GestureGenerator gesture;
    nRetVal = gesture.Create(context);
    CHECK_RC(nRetVal, "Initialize Gesture node");
    
    // Initialize NITE session manager
    nRetVal = sessionManager.Initialize(&context, "Wave,Click", "RaiseHand");
    CHECK_RC(nRetVal, "Initialize session manager");
    
    sessionManager.RegisterSession(NULL, &SessionStart, &SessionEnd, &SessionProgress);
    
    
    // Start generating data
    nRetVal = context.StartGeneratingAll();
    CHECK_RC(nRetVal, "Start generating data");
    
    
    while(run) {
        // Wait for new data
        nRetVal = context.WaitAnyUpdateAll();
        CHECK_RC(nRetVal, "Wait for new data");
        sessionManager.Update(&context);
        std::cout << "test" << std::endl;
    }
    
    
    
	
	return 0;
}