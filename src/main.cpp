#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
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

using namespace std;

int cnt;
int latency;
int lastX;
int lastY;
int lastZ;
int input_offset_x;
int input_offset_y;


double input_scale = 1.5;
double max_path_block = 0.5;

bool latencyChanged = false;
bool run = true;
bool measure = false;

clock_t latencytimer;
time_t prevtime = time(0);

std::queue<XnPoint3D> coordinateQueue;
std::vector<pair<double, double> > path_coordinates;
std::queue<pair<double, double> > checkpoints;

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

void
getMeasurements()
{
	double closestDist = INT_MAX;
	pair<double, double> closestPoint;
	pair<double, double> secondPoint;
	double X = mappedPos[0];
	double Y = mappedPos[1];

	int index = 0;

	for (int i = 0; i < path_coordinates.size(); i++) {
		double length = sqrt(pow(X - path_coordinates.at(i).first, 2.0) +
				pow(Y - path_coordinates.at(i).second, 2.0));

		if (length < closestDist) {
			closestDist = length;
			index = i;
		}
	}

	closestPoint = path_coordinates.at(index);
	
	double prevLength = INT_MAX;
	double nextLength = INT_MAX;

	if (index > 0) {
		prevLength = sqrt(pow(X - path_coordinates.at(index-1).first, 2.0) + pow(Y - path_coordinates.at(index-1).second, 2.0));
	}

	if (index < path_coordinates.size() - 1) {
		nextLength = sqrt(pow(X - path_coordinates.at(index+1).first, 2.0) + pow(Y - path_coordinates.at(index+1).second, 2.0));
	}

	if (index > 0 && prevLength < nextLength) {
		secondPoint = path_coordinates.at(index-1);
	}
	else if (index < path_coordinates.size() - 1) {
		secondPoint = path_coordinates.at(index+1);
	}

	double segmentLength = sqrt(pow(closestPoint.first - secondPoint.first, 2.0) + pow(closestPoint.second - secondPoint.second, 2.0));

	double dx = closestPoint.first - secondPoint.first;
	double dy = closestPoint.second - secondPoint.second;

	pair<double, double> normal = make_pair(-dy, dx);
	pair<double, double> point = make_pair(X - closestPoint.first, Y - closestPoint.second);

	double resultLength;

	if (segmentLength > 0) {
		resultLength = (normal.first*point.first + normal.second*point.second) / segmentLength;
	}

	cout << resultLength << endl;
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
	//if (print)
	//	printf(">>> %f\n", z);

	//std::cout << "X: " << lastX << "\t Y: " << lastY << "\t Z: " << lastZ <<"\t Latency: " << latency << "ms" << std::endl;
	if (measure) {
		getMeasurements();
	}
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
mouse_func(int button, int st, int x, int y)
{
	mx = x;
	my = y;
	
	lastX = mx;
	lastY = my;

	if (st != GLUT_UP)
		return;

	switch (button) {

	//mouse key down
	case GLUT_LEFT_BUTTON:
		printf("mouse: %d - %d ------------- window: %f - %f -%f\n", x, y, mappedPos[0], mappedPos[1], mappedPos[2]);
		print = !print;
		break;

	}
}

static void
motion_func(int x, int y)
{
	mx = x;
	my = y;
	
	lastX = mx;
	lastY = my;
}

static void
passive_func(int x, int y)
{
	float z;

	mx = x;
	my = y;
	
	lastX = mx;
	lastY = my;

	glReadPixels(x, viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	gluUnProject((float) x, (float) (viewport[3] - y), 1.0, modelview, projection, viewport, &mappedPos[0], &mappedPos[1], &mappedPos[2]);
	//if (print)
	//	printf(">>> %f\n", z);
}

/* Will read path_coordinates to checkpoints */
void
read_checkpoints()
{
	while (checkpoints.size() > 1)
		checkpoints.pop();

	for (int i=0; i < path_coordinates.size(); i++)
		checkpoints.push(path_coordinates.at(i));
}

/* Mark checkpoint visited. checkpoints is FIFO, thus first element will be removed. */
void
visit_checkpoint()
{
	if (checkpoints.size() < 1)
		return;

	fprintf(stdout, "Will remove: %.2f, %.2f\n", checkpoints.front().first, checkpoints.front().second);
	checkpoints.pop();
}

double
checkpoint_coordinate_distance(pair<double, double> p1, pair<double, double> p2)
{
	double xdelta = p1.first - p2.first;
	double ydelta = p1.second - p2.second;

	return sqrt((xdelta * xdelta) + (ydelta * ydelta));
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

void
startMeasuring()
{
	measure = true;
}

void
stopMeasuring()
{
	measure = false;
}

void
readCoordinates(const string filename)
{
	double curvex, curvey;
	ifstream input;
	
	path_coordinates.clear();

	input.open(filename.c_str(), ios::in);

	if (!input) {
		cout << "Error opening file: " << filename << endl;
		exit(1);
	}

	while (input >> curvex >> curvey) {
		if (path_coordinates.size() > 0) {
			double tmplength = sqrt(pow(path_coordinates.back().first - curvex, 2.0)
					+ pow(path_coordinates.back().second - curvey, 2.0));
			
			double prevx = path_coordinates.back().first;
			double prevy = path_coordinates.back().second;

			double dirx = -1;
			double diry = -1;

			double stepx = (path_coordinates.back().first - curvex) / tmplength * max_path_block * dirx;
			double stepy = (path_coordinates.back().second - curvey)  / tmplength * max_path_block * diry;

			int steps = tmplength / max_path_block;

			if (steps > 0) {
				for (int i = 0; i < steps; i++) {
					prevx += stepx;
					prevy += stepy;
					path_coordinates.push_back(make_pair(prevx, prevy));
				}
			}

			if (prevx != curvex) {
				path_coordinates.push_back(make_pair(curvex, curvey));
			}
		}
		else {
			path_coordinates.push_back(make_pair(curvex, curvey));
		}
	}

	input.close();
}

/*
 * Scene 1, straight line and the ring
 * */
void
initScene1()
{
	const float width = 5.0;

	for (vector<pair<double, double> >::iterator i = path_coordinates.begin(); i != path_coordinates.end(); i++) {
		path.AddPoint(Point3((*i).first, (*i).second, 0.0));
	}

//	path.AddPoint(Point3(-width, 0.0, 0.0));
//	path.AddPoint(Point3(width, 0.0, 0.0));

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
main(int argc, char *argv[])
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

	readCoordinates("../data/curve1.txt");
	read_checkpoints();
	initScene1();
	
	startMeasuring();
	
	input_offset_x = win_x / 2;
	input_offset_y = win_y / 2;

	// Initialize context object
	nRetVal = context.Init();
	CHECK_RC(nRetVal, "Initialize context");

	xn::DepthGenerator depth;
	nRetVal = depth.Create(context);
	
	// Let it draw the picture without kinect attached
	if (nRetVal == XN_STATUS_OK) {
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
		glutIdleFunc(update);
	}
	else {
		glutMouseFunc(mouse_func);
		glutMotionFunc(motion_func);
		glutPassiveMotionFunc(passive_func);
	}

	glutKeyboardFunc(key_func);
	glutSpecialFunc(special_key_func);
	glutReshapeFunc(reshape_func);
	glutTimerFunc(TIMER_FUNC_STEP, timer_func, 0);
	glutDisplayFunc(display_func);
	glutMainLoop(); 

	return 0;
}
