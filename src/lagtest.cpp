#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <ctime>
#include <climits>

#define KINECT_HZ 30

using namespace std;

class pPosition
{
public:
	pPosition(float = 0, float = 0, float = 0);
	float X;
	float Y;
	float Z;
};

pPosition::pPosition(float x, float y, float z)
{
	X = x;
	Y = y;
	Z = z;
}

int
main(int argc, const char *argv[])
{
	unsigned int sleeptime;
	size_t table_size;
	size_t i;
	pPosition *t = NULL;

	/*
	 * Kinect receives input with 30Hz rate, thus 2 sec needs
	 * space for 2 * 30 = 60 values. Therefore 1,5 sec will need
	 * 1,5 * 30 = 45.
	 */
	sleeptime = (argv[1] != NULL) ?
		(atof(argv[1]) * 1000000) : 1500000.0;
	table_size = (sleeptime / 1000000.0) * KINECT_HZ;
	t = new pPosition[table_size];
	i = 0;

	fprintf(stdout, "Table size: %d (%.2f seconds)\n", table_size,
			(sleeptime / 1000000.0));

	(void)srand((unsigned)time(0));
	while (true) {
		usleep(sleeptime);
		t[i].X = (float)rand() / RAND_MAX;
		t[i].Y = (float)rand() / RAND_MAX;
		t[i].Z = (float)rand() / RAND_MAX;
		fprintf(stdout, "%.2f, %.2f, %.2f\n", t[i].X, t[i].Y, t[i].Z);
		i++;

		/* 
		 * Empty buffer here.
		 * 
		 */
		if (i + 1 >= table_size) {
			for (unsigned int j=0; j < table_size; j++) {
				t[j].X = 0.0;
				t[j].Y = 0.0;
				t[j].Z = 0.0;
			}
			i = 0;
			fprintf(stdout, "Table cleared\n");
		}
	}

	if (t)
		delete [] t;

	return 0;
}
