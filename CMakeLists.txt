cmake_minimum_required(VERSION 3.16)
project(STLViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/stlviewer.cpp
    src/stlloader.cpp
)

set(HEADERS
    include/mainwindow.h
    include/stlviewer.h
    include/stlloader.h
)

add_executable(STLViewer ${SOURCES} ${HEADERS})

target_include_directories(STLViewer PRIVATE include)

target_link_libraries(STLViewer 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGL 
    Qt6::OpenGLWidgets
)

# Copy example STL files if they exist
file(GLOB STL_FILES "examples/*.stl")
if(STL_FILES)
    file(COPY ${STL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/examples/)
endif()
