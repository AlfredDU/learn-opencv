project(Euler)
cmake_minimum_required(VERSION 3.0)
aux_source_directory(. SRC_LIST)

# Enable C++ 11 support.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Find OpenCV
find_package(OpenCV REQUIRED)

# Output OpenCV related variables
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Add OpenCV headers location to your include paths
include_directories(${OpenCV_INCLUDE_DIRS})

# Declare the executable target built from your sources
add_executable(${PROJECT_NAME} ${SRC_LIST})

# Link your application with OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Add resources files
configure_file(perpetual-motion.jpg perpetual-motion.jpg COPYONLY)