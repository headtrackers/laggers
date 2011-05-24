#include <XnCppWrapper.h>
#include "XnVNite.h"
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <ctime>
#include <queue>
#include <utility>

// This macro checks the return code that all OpenNI calls make
// and throws an error if the return code is an error. Use this
// after all calls to OpenNI objects. Great for debugging.
//
// This macro is copied from here:
// http://www.codingbeta.com/?p=24
//
#define CHECK_RC(rc, what) \
if (rc != XN_STATUS_OK) \
{ \
printf("%s failed: %s\n", what, xnGetStatusString(rc)); \
return rc; \
}

int cnt;
int latency;
int lastX;
int lastY;
int lastZ;
bool latencyChanged = false;
bool run = true;
clock_t latencytimer;
time_t prevtime = time(0);
std::queue<XnPoint3D> coordinateQueue;

XnStatus nRetVal = XN_STATUS_OK;
XnVSessionManager sessionManager;
xn::Context context;

cnt = latency = lastX = lastY = lastZ = 0;

void
XN_CALLBACK_TYPE HandUpdate(xn::HandsGenerator &generator, XnUserID user,
        const XnPoint3D *pPosition, XnFloat fTime, void *pCookie)
{
    XnPoint3D curPoint;

    curPoint.X = pPosition->X;
    curPoint.Y = pPosition->Y;
    curPoint.Z = pPosition->Z;

    time_t tmptime = time(0);
    clock_t curtime = clock();

    coordinateQueue.push(curPoint);

    if (latencyChanged && difftime(curtime, latencytimer) / 100 > latency) {
        latencyChanged = false;
    }

    if (latencyChanged == false) {
        lastX = coordinateQueue.front().X;
        lastY = coordinateQueue.front().Y;
        lastZ = coordinateQueue.front().Z;

        coordinateQueue.pop();
    }

    std::cout << "X: " << lastX << "\t Y: " << lastY << "\t Z: " << lastZ <<"\t Latency: " << latency << "ms" << std::endl;
//    fprintf(stdout, "%ld: X: %-15f Y: %-15f Z: %-15f\n", tmptime, pPosition->X, pPosition->Y, pPosition->Z);
}

void
changeLatency()
{
    coordinateQueue = std::queue<XnPoint3D>();
    latencytimer = clock();
    latencyChanged = true;
}

void
XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus,
        const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt)
{
    fprintf(stdout, "Session progress (%6.2f,%6.2f,%6.2f) - %6.2f [%s]\n",
            ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z, fProgress,  strFocus);
}

void
XN_CALLBACK_TYPE SessionStart(const XnPoint3D& pFocus, void* UserCxt)
{
    std::cout << "Session started" << std::endl;
    return;
}

void
XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
    return;
}

void
update()
{
    nRetVal = context.WaitAnyUpdateAll();
    CHECK_RC(nRetVal, "Wait for new data");
    sessionManager.Update(&context);
}

void
key_func(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
        case 'Q':
            exit(0);
            break;
        case '+':
            latency += 25;
            changeLatency();
            break;
        case '-':
            if(latency > 0) {
                latency -= 25;
            }
            changeLatency();
            break;
    }
}

static void
draw()
{
    glClear ( GL_COLOR_BUFFER_BIT );
    glutSwapBuffers();
}

static void
initGraphics()
{
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Spanners");

    glViewport ( 0, 0, 640, 480 );
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity ();
    gluOrtho2D ( 0.0f, 640, 0.0f, 480 );
    glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear ( GL_COLOR_BUFFER_BIT );
}

int
main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    initGraphics();
    glutDisplayFunc(&draw);
    glutIdleFunc(&update);
    glutKeyboardFunc(&key_func);

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

    XnCallbackHandle handPositionCallBack;
    hands.RegisterHandCallbacks(NULL, &HandUpdate, NULL, NULL, handPositionCallBack);

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

    glutMainLoop(); 

    return 0;
}
