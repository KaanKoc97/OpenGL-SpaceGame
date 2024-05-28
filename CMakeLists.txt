cmake_minimum_required(VERSION 3.0)
project(MyOpenGLProject)

# Copy font file to build directory
configure_file("/home/kaankoc/Projects/OpenGL-SpaceGame/include/fonts/space.ttf" "${CMAKE_CURRENT_BINARY_DIR}/space.ttf" COPYONLY)

# Set the path to the GLFW include directory
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/GLFW)

# Add the GLFW include directory to the project
include_directories(${GLFW_INCLUDE_DIR})

# Set the path to the GLFW library directory
set(GLFW_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/GLFW)

# Add FreeType include directory
set(FREETYPE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/freetype)
include_directories(${FREETYPE_INCLUDE_DIR})

# Add OpenGL include directory
set(OpenGL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/opengl)
include_directories(${OpenGL_INCLUDE_DIR})

# Find and include GLFW
find_package(GLFW REQUIRED)
if (GLFW_FOUND)
    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories(${GLFW_LIBRARIES})
else()
    message(FATAL_ERROR "GLFW not found!")
endif()

# Find and include OpenGL
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
else()
    message(FATAL_ERROR "OpenGL not found!")
endif()

# Add your source files here
set(SOURCES
    main.cpp
    glad.c
    # Add other source files as needed
)

# Create an executable target
add_executable(MyExecutable ${SOURCES})

# Link against libraries
target_link_libraries(MyExecutable ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} freetype)

# Optionally, if you need to link against additional libraries, add them here
# target_link_libraries(MyExecutable additional_library)
