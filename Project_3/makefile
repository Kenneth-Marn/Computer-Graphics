GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW 
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3`


CC = g++
EXE = assign3
OBJS = main.o shader.o tiny_obj_loader.o Viewer.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o: main.cpp 
	$(CC) $(CPPFLAGS) -c main.cpp

shader.o : shader.cpp shader.hpp
	$(CC) $(CPPFLAGS) -c shader.cpp

tiny_obj_loader.o: tiny_obj_loader.h tiny_obj_loader.cc
	$(CC) $(CPPFLAGS) -c tiny_obj_loader.cc

Viewer.o: Viewer.hpp Viewer.cpp
	$(CC) $(CPPFLAGS) -c Viewer.hpp Viewer.cpp


clean:
	rm -f *.o $(EXE)$(EXT)