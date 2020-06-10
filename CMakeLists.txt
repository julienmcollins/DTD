cmake_minimum_required(VERSION 3.10)

## Set the project name
project(DTD VERSION 1.0)

## Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compile for windows?
# set(CMAKE_SYSTEM_NAME WindowsStore)
# set(CMAKE_SYSTEM_VERSION 10.0)

## Set cmake path for sdl2 modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

## Set cmake path for toolchain
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cross_compile.cmake)

## Include directories
include_directories(${CMAKE_SOURCE_DIR})

## Source files
# Development
SET(DEVELOPMENT_SOURCE QuiteGoodMachine/Source/Development/Public/DevelZoom.cpp)

# GameManager
SET(GAMEMANAGER_SOURCE QuiteGoodMachine/Source/GameManager/Public/Application.cpp
                       QuiteGoodMachine/Source/GameManager/Public/main.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Level.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Notebook.cpp
                       QuiteGoodMachine/Source/GameManager/Public/KeyHandler.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Timers/SDLTimer.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Timers/FPSTimer.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Timers/SecondsTimer.cpp
                       QuiteGoodMachine/Source/GameManager/Public/StateSystem/DrawState.cpp
                       QuiteGoodMachine/Source/GameManager/Public/StateSystem/StateContext.cpp
                       QuiteGoodMachine/Source/GameManager/Public/StateSystem/PlayerStates.cpp
                       QuiteGoodMachine/Source/GameManager/Public/StateSystem/EnemyStates.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/BirdMaster.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/Correspondence.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/Correspondent.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/PigeonPost.cpp
   )

# ObjectManager
SET(OBJECTMANAGER_SOURCE QuiteGoodMachine/Source/ObjectManager/Public/Element.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Enemy.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Entity.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Global.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Object.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Elements/PositionalElement.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Elements/DrawableElement.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Elements/TangibleElement.cpp
   )
               
# RenderingEngine
SET(RENDERINGENGINE_SOURCE QuiteGoodMachine/Source/RenderingEngine/Public/BasicShaderProgram.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/DebugDraw.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/GLData.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/RenderingEngine.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/ShaderProgram.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/Texture.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/Animation.cpp
   )

# MemoryManager
SET(MEMORYMANAGER_SOURCE QuiteGoodMachine/Source/MemoryManager/Public/ObjectManager.cpp
)

# HUD
SET(HUD_SOURCE QuiteGoodMachine/Source/HUD/Public/HUDElement.cpp
               QuiteGoodMachine/Source/HUD/Public/HUD.cpp
               QuiteGoodMachine/Source/HUD/Public/PlayerLife.cpp
)

# Set sources
SET(SOURCES ${DEVELOPMENT_SOURCE} ${GAMEMANAGER_SOURCE} ${OBJECTMANAGER_SOURCE} ${RENDERINGENGINE_SOURCE} ${MEMORYMANAGER_SOURCE} ${HUD_SOURCE})

## Header files
# Development
SET(DEVELOPMENT_HEADER QuiteGoodMachine/Source/Development/Private/DevelZoom.h
)

# GameManager
SET(GAMEMANAGER_HEADER QuiteGoodMachine/Source/GameManager/Private/Application.h
                       QuiteGoodMachine/Source/GameManager/Private/Level.h
                       QuiteGoodMachine/Source/GameManager/Private/Notebook.h
                       QuiteGoodMachine/Source/GameManager/Private/ContactFilter.h
                       QuiteGoodMachine/Source/GameManager/Private/ContactListener.h
                       QuiteGoodMachine/Source/GameManager/Private/DoOnce.h
                       QuiteGoodMachine/Source/GameManager/Private/KeyHandler.h
                       QuiteGoodMachine/Source/GameManager/Private/ClassType.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/TimerInterface.h
                       QuiteGoodMachine/Source/GameManager/Private/Timers/SDLTimer.h
                       QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h
                       QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/StateInterface.h
                       QuiteGoodMachine/Source/GameManager/Private/StateSystem/DrawState.h
                       QuiteGoodMachine/Source/GameManager/Private/StateSystem/StateContext.h
                       QuiteGoodMachine/Source/GameManager/Private/StateSystem/PlayerStates.h
                       QuiteGoodMachine/Source/GameManager/Private/StateSystem/EnemyStates.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/BirdMasterInterface.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/CorrespondentInterface.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/PigeonPostInterface.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/BirdMaster.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h
   )

# MathStructures
SET(MATHSTRUCTURES_HEADER QuiteGoodMachine/Source/MathStructures/Private/Coordinates.h
                          QuiteGoodMachine/Source/MathStructures/Private/Vector.h
   )

# ObjectManager
SET(OBJECTMANAGER_HEADER QuiteGoodMachine/Source/ObjectManager/Private/Element.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Entity.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Global.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Object.h
                         QuiteGoodMachine/Source/ObjectManager/Interfaces/Component.h
                         QuiteGoodMachine/Source/ObjectManager/Interfaces/ElementInterface.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Elements/PositionalElement.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Elements/DrawableElement.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Elements/TangibleElement.h
   )
               
# RenderingEngine
SET(RENDERINGENGINE_HEADER QuiteGoodMachine/Source/RenderingEngine/Private/BasicShaderProgram.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/DebugDraw.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/GLData.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/ShaderProgram.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h
   )

# MemoryManager
SET(MEMORYMANAGER_HEADER QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h
                         QuiteGoodMachine/Source/MemoryManager/Interfaces/MemoryManagerInterface.h
   )

# HUD
SET(HUD_HEADER QuiteGoodMachine/Source/HUD/Interfaces/HUDElementInterface.h
               QuiteGoodMachine/Source/HUD/Interfaces/HUDInterface.h
               QuiteGoodMachine/Source/HUD/Private/HUDElement.h
               QuiteGoodMachine/Source/HUD/Private/HUD.h
               QuiteGoodMachine/Source/HUD/Private/PlayerLife.h
   )

# Set sources
SET(HEADERS ${DEVELOPMENT_HEADER} ${GAMEMANAGER_HEADER} ${MATHSTRUCTURES_HEADER} ${OBJECTMANAGER_HEADER} ${RENDERINGENGINE_HEADER} ${MEMORYMANAGER_HEADER} ${HUD_HEADER})

## Find packages
# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})
SET(SDL2_LIBRARY_LIST ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# GLM
find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

# GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# Compile libraries
SET(LIBRARIES ${SDL2_LIBRARY_LIST} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} ${GLEW_LIBRARIES})

# Set debug
set(CMAKE_BUILD_TYPE Debug)

# Find static libraries
find_library(LIBBOX2D Box2D .)
find_library(LIBSOIL SOIL .)

## Add the executable
add_executable(dtd ${HEADERS} ${SOURCES})
target_link_libraries(dtd ${LIBRARIES} ${LIBBOX2D} ${LIBSOIL})