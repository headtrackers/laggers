#include <ctime>
#include <iostream>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <XnCppWrapper.h>

#include "XnVNite.h"
#include "Camera.h"
#include "Definitions.h"
#include "Cylinder.h"
#include "Donut.h"
#include "Path.h"

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
int input_offset_x;
int input_offset_y;


double input_scale = 2.0;

bool latencyChanged = false;
bool run = true;
clock_t latencytimer;
time_t prevtime = time(0);
std::queue<XnPoint3D> coordinateQueue;

XnStatus nRetVal = XN_STATUS_OK;
XnVSessionManager sessionManager;
xn::Context context;

static int N;
static float dt, d;
static int dsim;
static int dump_frames;
static int frame_number;

static int viewport[4];
static double modelview[16];
static double projection[16];

static int win_id;
static int win_x, win_y;
static int omx, omy, mx, my;
static double mappedPos[3];

static Camera camera;
static Donut ring;
static Path path;

static bool print = false;

static float light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
static float light_ambient[] = { .2, .2, .2, 1.0 };
static float light_position[] = { 1.0, 1.0, -4.0, 1.0 }; /* Infinite light location. */

void
ViewOrtho()
{
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_LIGHTING);
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void
ViewPerspective()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void
changeLatency()
{
	coordinateQueue = std::queue<XnPoint3D>();
	latencytimer = clock();
	latencyChanged = true;
}

/*
 ----------------------------------------------------------------------
 OpenGL specific drawing routines
 ----------------------------------------------------------------------
 */

static void
post_display(void)
{
	frame_number++;
	glFlush(); //Finish rendering
	glutSwapBuffers();
}

/*
 ----------------------------------------------------------------------
 Kinect callback routines
 ----------------------------------------------------------------------
 */

void
XN_CALLBACK_TYPE HandUpdate(xn::HandsGenerator &generator, XnUserID user, const XnPoint3D *pPosition, XnFloat fTime, void *pCookie)
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
		lastX = ((-1) * coordinateQueue.front().X / input_scale) + input_offset_x;
		lastY = ((-1) * coordinateQueue.front().Y / input_scale) + input_offset_y;
		lastZ = coordinateQueue.front().Z;

		coordinateQueue.pop();
	}

	float z;
	glReadPixels(lastX, viewport[3] - lastY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	gluUnProject((float) lastX, (float) (viewport[3] - lastY), 1.0, modelview, projection, viewport, &mappedPos[0], &mappedPos[1], &mappedPos[2]);
	if (print)
		printf(">>> %f\n", z);

	std::cout << "X: " << lastX << "\t Y: " << lastY << "\t Z: " << lastZ <<"\t Latency: " << latency << "ms" << std::endl;
//	fprintf(stdout, "%ld: X: %-15f Y: %-15f Z: %-15f\n", tmptime, pPosition->X, pPosition->Y, pPosition->Z);
}

void
XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt)
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

/*
 ----------------------------------------------------------------------
 GLUT callback routines
 ----------------------------------------------------------------------
 */

void update()
{
	nRetVal = context.WaitAnyUpdateAll();
	if (nRetVal != XN_STATUS_OK) {
		fprintf(stdout, "%s failed: %s\n", "Wait for new data", xnGetStatusString(nRetVal));
	}
	//CHECK_RC(nRetVal, "Wait for new data");
	sessionManager.Update(&context);
}

void
key_func(unsigned char key, int x, int y)
{
	switch (key) {
		case 'q':
		case 'Q':
			context.Shutdown();
			exit(0);
			break;
		case '0':
			camera.Reset();
			break;

			//camera movement cases
		case '+':
			camera.MoveZ(MOVE_AMT);
			break;
		case '-':
			camera.MoveZ(-MOVE_AMT);
			break;

			//camera rotation cases
		case 'd':
			camera.RotateY(ROT_AMT);
			break;
		case 'g':
			camera.RotateY(-ROT_AMT);
			break;

		case 'r':
			camera.RotateX(-ROT_AMT);
			break;
		case 'f':
			camera.RotateX(ROT_AMT);
			break;
		case 'e':
			camera.RotateZ(ROT_AMT);
			break;
		case 't':
			camera.RotateZ(-ROT_AMT);
			break;

		case '1':
			break;
		case '3':
			break;
		case 'a':
			latency += 25;
			changeLatency();
			break;
		case 's':
			if(latency > 0) {
				latency -= 25;
			}
			changeLatency();
			break;
	}
}

static void
special_key_func(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			camera.MoveY(-MOVE_AMT);
			break;
		case GLUT_KEY_DOWN:
			camera.MoveY(MOVE_AMT);
			break;

		case GLUT_KEY_LEFT:
			camera.MoveX(MOVE_AMT);
			break;
		case GLUT_KEY_RIGHT:
			camera.MoveX(-MOVE_AMT);
			break;
	}
}



static void
passive_func(int x, int y)
{
	float z;

	mx = x;
	my = y;

	glReadPixels(x, viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	gluUnProject((float) x, (float) (viewport[3] - y), 1.0, modelview, projection, viewport, &mappedPos[0], &mappedPos[1], &mappedPos[2]);
	if (print)
		printf(">>> %f\n", z);
}

static void
reshape_func(int x, int y)
{
	if (y == 0 || x == 0)
		return; //Nothing is visible then, so return

	//Set a new projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//Angle of view:40 degrees
	//Near clipping plane distance: 0.5
	//Far clipping plane distance: 20.0
	gluPerspective(45.0, (GLdouble) x / (GLdouble) y, 1.5, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, x, y); //Use the whole window for rendering

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

static void
timer_func(int amt)
{
	ring.SetPosition(Vector3(-mappedPos[0], -mappedPos[1], 0.0f));

	glutPostRedisplay();
	glutTimerFunc(TIMER_FUNC_STEP, timer_func, 0);
}

static void
display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	camera.Init();

	glPushMatrix(); //render
	glTranslatef(0.0, 0.0, ZOFFSET);
	glScalef(0.1, 0.1, 0.1);
	glColor3f(0.8f, 0.2f, 0.2f);
	//glutSolidCube(3.0);
	ring.Render();
	path.Render();
	glPopMatrix();

	ViewPerspective();

	post_display();
}

/*
 * Scene 1, straight line and the ring
 * */
void
initScene1()
{
	const float width = 5.0;

	path.AddPoint(Point3(-width, 0.0, 0.0));
	path.AddPoint(Point3(width, 0.0, 0.0));

	ring.SetPosition(Vector3(width + 1.0f, 0.0f, 0.0f));
}

/*
 ----------------------------------------------------------------------
 open_glut_window --- open a glut compatible window and set callbacks
 ----------------------------------------------------------------------
 */

static void
init(void)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //For animations you should use double buffering
	glutInitWindowSize(win_x, win_y);
	//Create a window with rendering context and everything else we need
	glutCreateWindow("IVE");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glClearColor(0.0, 0.0, 0.0, 0.0);

	/* Enable a single OpenGL light. */
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}

int
main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	if (argc == 1) {
		N = 64;
		dt = 0.5f;
		d = 5.f;
		fprintf(stderr, "Using defaults : N=%d dt=%g d=%g\n", N, dt, d);
	} else {
		N = atoi(argv[1]);
		dt = atof(argv[2]);
		d = atof(argv[3]);
	}

	dsim = 0;
	dump_frames = 0;
	frame_number = 0;

	win_x = 800;
	win_y = 600;
	init();
	initScene1();
	
	input_offset_x = win_x / 2;
	input_offset_y = win_y / 2;

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

	glutKeyboardFunc(key_func);
	glutSpecialFunc(special_key_func);
	glutReshapeFunc(reshape_func);
	glutTimerFunc(TIMER_FUNC_STEP, timer_func, 0);
	glutDisplayFunc(display_func);
	glutIdleFunc(update);
	glutMainLoop(); 

	return 0;
}
