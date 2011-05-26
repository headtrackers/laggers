CC=clang
MV=mv

all:
	${CC} *.cpp -o laggers -I /usr/include/ni -I /usr/include/nite\
		-lOpenNI -lglut -lXnVNite -lGLU
	${MV} laggers ../bin/

clean:
	rm -rf *.o laggers
