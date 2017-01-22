
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW 
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3`

CC = g++
EXE = assign4
OBJS = main.o Shader.o Viewer.o Skybox.o Table.o Sphere.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o: main.cpp Viewer.h Skybox.h Table.h SunMoon.h Sphere.hpp
	$(CC) $(CPPFLAGS) -c main.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Viewer.o : Viewer.cpp Viewer.h InputState.h
	$(CC) $(CPPFLAGS) -c Viewer.cpp
	
Skybox.o: Skybox.cpp Skybox.h stb_image.h 
	$(CC) $(CPPFLAGS) -c Viewer.cpp
	
Table.o: Table.cpp Table.h
	$(CC) $(CPPFLAGS) -c Table.cpp
	
Sphere.o : Sphere.hpp Sphere.cpp
	$(CC) $(CPPFLAGS) -c Sphere.cpp
	
SunMoon.o: SunMoon.cpp SunMoon.h stb_image.h  Sphere.hpp
	$(CC) $(CPPFLAGS) -c SunMoon.cpp
clean:
	rm -f *.o $(EXE)$(EXT)
