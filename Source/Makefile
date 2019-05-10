# PHONY
.PHONY: clean create all

#OBJS specifies which files to compile as part of the project
OBJS = Application.cpp Global.cpp Entity.cpp main.cpp Texture.cpp Timer.cpp Object.cpp DebugDraw.cpp Element.cpp Level.cpp Enemy.cpp

#CC specifies which compiler we're using
CC = g++
CCW32 = i686-w64-mingw32-gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image libBox2D.a

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
