cmake_minimum_required(VERSION 2.8)
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
PROJECT(DISPLAYIMAGE)
find_package(OpenCV REQUIRED)
add_executable( DisplayImage DisplayImage.cpp )
target_link_libraries( DisplayImage ${OpenCV_LIBS} )
