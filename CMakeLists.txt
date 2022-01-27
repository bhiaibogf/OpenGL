cmake_minimum_required(VERSION 3.16)
project(sketchfab)

set(CMAKE_CXX_STANDARD 17)

set(LIBS m Xrandr Xi X11 Xxf86vm pthread dl Xinerama Xcursor)

find_package(OpenGL REQUIRED)
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})
#set(LIBS ${LIBS} GL)

find_package(glfw3 3.3 REQUIRED)
set(LIBS ${LIBS} glfw)

find_package(glm REQUIRED)

find_package(ASSIMP REQUIRED)
set(LIBS ${LIBS} ${ASSIMP_LIBRARIES})

#message(${LIBS})

add_library(GLAD third_party/glad/glad.c)
target_link_libraries(GLAD dl)

add_library(STB_IMAGE "third_party/stb/stb_image.cpp")

file(GLOB IMGUI_SRC third_party/imgui/imgui*.cpp)
add_library(IMGUI ${IMGUI_SRC})

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(sketchfab ${SRC})
target_link_libraries(sketchfab ${LIBS} GLAD STB_IMAGE IMGUI)

include_directories(./third_party)
