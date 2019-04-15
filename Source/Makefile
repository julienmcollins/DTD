# PHONY
.PHONY: clean create all

#OBJS specifies which files to compile as part of the project
OBJS = Application.cpp Global.cpp Entity.cpp main.cpp Texture.cpp Timer.cpp Object.cpp DebugDraw.cpp Element.cpp Level.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image libBox2D.a

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = dtd 

# Default build
default : clean create

#This is the target that compiles our executable
create : $(OBJS)
		$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

# Clean target
clean :
	rm -rf dtd
