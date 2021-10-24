cmake_minimum_required(VERSION 3.16)
project(sketchfab)

set(CMAKE_CXX_STANDARD 17)

set(LIBS m Xrandr Xi X11 Xxf86vm pthread dl Xinerama Xcursor)

find_package(OpenGL REQUIRED)
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})
#set(LIBS ${LIBS} GL)

find_package(glfw3 3.3 REQUIRED)
set(LIBS ${LIBS} glfw)

#find_package(ASSIMP REQUIRED)

#message(${LIBS})

add_library(GLAD src/glad.c)
target_link_libraries(GLAD ${LIBS})

file(GLOB SRC "src/*.cpp")

add_executable(sketchfab ${SRC})
target_link_libraries(sketchfab ${LIBS} GLAD)

include_directories(./includes)
