CC=clang
MV=mv
MAKEDIR=mkdir
INCLUDES = -I /usr/include/ni\
		   -I /usr/include/nite\
		   -I /usr/local/include/libfreenect\
		   -I /usr/local/include/libfreenect
LIBS = -L /usr/local/lib -lfreenect -lOpenNI -lglut\
	   -lXnVNite -lGLU -lfreenect
FILES = `ls *.cpp`

all:
	${CC} $(INCLUDES) $(LIBS) $(FILES) -o laggers
	if ! test -d ../bin; then mkdir ../bin; fi
	${MV} laggers ../bin/

clean:
	rm -rf *.o laggers
