cmake_minimum_required(VERSION 3.10)

## Set the project name
project(DTD VERSION 1.0)

## Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

## Set cmake path for sdl2 modules
message("${CMAKE_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

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
                       QuiteGoodMachine/Source/GameManager/Public/State.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Timers/SDLTimer.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Timers/FPSTimer.cpp
                       QuiteGoodMachine/Source/GameManager/Public/Timers/SecondsTimer.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/BirdMaster.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/Correspondence.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/Correspondent.cpp
                       QuiteGoodMachine/Source/GameManager/Public/EventSystem/PigeonPost.cpp)

# ObjectManager
SET(OBJECTMANAGER_SOURCE QuiteGoodMachine/Source/ObjectManager/Public/Element.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Enemy.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Entity.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Global.cpp
                         QuiteGoodMachine/Source/ObjectManager/Public/Object.cpp)
               
# RenderingEngine
SET(RENDERINGENGINE_SOURCE QuiteGoodMachine/Source/RenderingEngine/Public/BasicShaderProgram.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/DebugDraw.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/GLData.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/RenderingEngine.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/ShaderProgram.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/Texture.cpp
                           QuiteGoodMachine/Source/RenderingEngine/Public/Animation.cpp)

# MemoryManager
SET(MEMORYMANAGER_SOURCE QuiteGoodMachine/Source/MemoryManager/Public/ObjectManager.cpp)

# HUD
SET(HUD_SOURCE QuiteGoodMachine/Source/HUD/Public/HUDElement.cpp
               QuiteGoodMachine/Source/HUD/Public/HUD.cpp)

# Set sources
SET(SOURCES ${DEVELOPMENT_SOURCE} ${GAMEMANAGER_SOURCE} ${OBJECTMANAGER_SOURCE} ${RENDERINGENGINE_SOURCE} ${MEMORYMANAGER_SOURCE} ${HUD_SOURCE})

## Header files
# Development
SET(DEVELOPMENT_HEADER QuiteGoodMachine/Source/Development/Private/DevelZoom.h)

# GameManager
SET(GAMEMANAGER_HEADER QuiteGoodMachine/Source/GameManager/Private/Application.h
                       QuiteGoodMachine/Source/GameManager/Private/Level.h
                       QuiteGoodMachine/Source/GameManager/Private/Notebook.h
                       QuiteGoodMachine/Source/GameManager/Private/State.h
                       QuiteGoodMachine/Source/GameManager/Private/Timers/SDLTimer.h
                       QuiteGoodMachine/Source/GameManager/Private/Timers/FPSTimer.h
                       QuiteGoodMachine/Source/GameManager/Private/Timers/SecondsTimer.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/BirdMasterInterface.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/CorrespondentInterface.h
                       QuiteGoodMachine/Source/GameManager/Interfaces/PigeonPostInterface.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/BirdMaster.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondence.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/Correspondent.h
                       QuiteGoodMachine/Source/GameManager/Private/EventSystem/PigeonPost.h)

# MathStructures
SET(MATHSTRUCTURES_HEADER QuiteGoodMachine/Source/MathStructures/Private/Coordinates.h)

# ObjectManager
SET(OBJECTMANAGER_HEADER QuiteGoodMachine/Source/ObjectManager/Private/Element.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Enemy.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Entity.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Global.h
                         QuiteGoodMachine/Source/ObjectManager/Private/Object.h)
               
# RenderingEngine
SET(RENDERINGENGINE_HEADER QuiteGoodMachine/Source/RenderingEngine/Private/BasicShaderProgram.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/DebugDraw.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/GLData.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/RenderingEngine.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/ShaderProgram.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/Texture.h
                           QuiteGoodMachine/Source/RenderingEngine/Private/Animation.h)

# MemoryManager
SET(MEMORYMANAGER_HEADER QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h
                         QuiteGoodMachine/Source/MemoryManager/Interfaces/MemoryManagerInterface.h)

# HUD
SET(HUD_HEADER QuiteGoodMachine/Source/HUD/Interfaces/HUDElementInterface.h
               QuiteGoodMachine/Source/HUD/Interfaces/HUDInterface.h
               QuiteGoodMachine/Source/HUD/Private/HUDElement.h
               QuiteGoodMachine/Source/HUD/Private/HUD.h)

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

# Find static libraries
find_library(LIBBOX2D Box2D .)
find_library(LIBSOIL SOIL .)

## Add the executable
add_executable(dtd ${HEADERS} ${SOURCES})
target_link_libraries(dtd ${LIBRARIES} ${LIBBOX2D} ${LIBSOIL})