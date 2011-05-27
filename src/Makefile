CC=clang
MV=mv
CFLAGS=-fPIC `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
INCLUDE = -I/usr/local/include/libfreenect
FREE_LIBS = -L/usr/local/lib -lfreenect

all:
	#${CC} *.cpp tilt.c -o laggers -I /usr/include/ni -I /usr/include/nite\
	#	-I /usr/local/include/libfreenect\
	#	-L /usr/local/lib -lOpenNI -lglut -lXnVNite -lGLU -lfreenect
	${CC} *.cpp -o laggers -I /usr/include/ni -I /usr/include/nite\
		-lOpenNI -lglut -lXnVNite -lGLU -lfreenect
	${MV} laggers ../bin/

clean:
	rm -rf *.o laggers
