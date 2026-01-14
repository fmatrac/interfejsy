cmake_minimum_required(VERSION 3.16)
project(MaskOverlay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML (obsługa wersji 2.5+ oraz 3.0+)
find_package(SFML 3 COMPONENTS Graphics Window System QUIET)
if(NOT SFML_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/ImageProcessor.cpp
    src/BlendMode.cpp
    src/MaskLibrary.cpp
    src/GUI.cpp
)

set(HEADERS
    include/Application.h
    include/ImageProcessor.h
    include/BlendMode.h
    include/MaskLibrary.h
    include/GUI.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link SFML (różne nazwy dla wersji 2.x i 3.x)
if(SFML_VERSION VERSION_GREATER_EQUAL 3)
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
endif()

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/masks DESTINATION ${CMAKE_BINARY_DIR})

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
