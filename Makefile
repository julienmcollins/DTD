# PHONY
.PHONY: clean create all

#OBJS specifies which files to compile as part of the project
OBJS = $(wildcard Source/*/Public/*.cpp)

#CC specifies which compiler we're using
CC = g++ -I. -I./Source/ObjectManager/ -I./Source/RenderingEngine/ -I./Source/GameEngine/
CCW32 = i686-w64-mingw32-gcc -I. -I./Source/ObjectManager/ -I./Source/RenderingEngine/ -I./Source/GameEngine/

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -g

#OpenGL flags
OPENGL_FLAGS = -lSOIL -lGL -lGLU $(shell pkg-config glew --libs)

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_mixer -lSDL2_image $(OPENGL_FLAGS) libBox2D.a libSOIL.a

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = dtd
OBJ_NAME_W = dtd.exe

# Default build
default : clean create

# This is the target that compiles our executable
create : $(OBJS)
		$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# This target will compile for windows
windows: $(OBJS)
		$(CCW32) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME_W)

# Clean target
clean :
	rm -rf dtd
