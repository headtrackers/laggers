CC=clang
MV=mv
CLAGS = `pkg-config --cflags opencv`
INCLUDES = -I /usr/include/ni\
		   -I /usr/include/nite\
		   -I /usr/local/include/libfreenect\
		   -I /usr/include/libusb-1.0\
		   -I /usr/local/include/libfreenect
LIBS = -L /usr/local/lib -lfreenect -lOpenNI -lglut\
	   -lXnVNite -lGLU -lfreenect -lusb1-0 -lusb
FILES = `ls *.cpp *.c` 

all:
	${CC} $(CFLAGS) $(INCLUDES) $(LIBS) $(FILES) -o laggers
	#${CC} *.cpp -o laggers -I /usr/include/ni -I /usr/include/nite\
	#	-lOpenNI -lglut -lXnVNite -lGLU -lfreenect
	${MV} laggers ../bin/

clean:
	rm -rf *.o laggers
