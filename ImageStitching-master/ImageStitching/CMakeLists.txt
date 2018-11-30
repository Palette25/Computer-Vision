cmake_minimum_required(VERSION 3.10)
project(ImageStitching)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

link_directories($ENV{VLROOT}/bin/maci64/)

if (APPLE)
    set (CMAKE_CXX_FLAGS "-lX11 -L/usr/X11/lib -I/usr/X11/include -I$ENV{VLROOT} -L$ENV{VLROOT}/bin/maci64/ -lvl")
    add_executable(panorama-mac
            CImg.h main.cpp spherical_warp.cpp spherical_warp.h feature_extract.cpp feature_extract.h point.cpp point.h utils.cpp utils.h feature_matching.cpp feature_matching.h)
    configure_file(libvl.dylib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libvl.dylib COPYONLY)
    configure_file(libomp.dylib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libomp.dylib COPYONLY)
    target_link_libraries(panorama-mac libvl.dylib)
endif (APPLE)

if(UNIX AND NOT APPLE)
    add_executable(panorama-linux
            CImg.h main.cpp spherical_warp.cpp spherical_warp.h feature_extract.cpp feature_extract.h point.cpp point.h utils.cpp utils.h feature_matching.cpp feature_matching.h)
    set (CMAKE_CXX_FLAGS "-lX11 -pthread")
    message(STATUS "Looking for X11...")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    configure_file(libvl.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libvl.so COPYONLY)
    target_link_libraries(panorama-linux libvl.so)
    target_link_libraries(panorama-linux ${X11_LIBRARIES})
endif(UNIX AND NOT APPLE)

if (WIN32)
    set (CMAKE_CXX_FLAGS "-lgdi32 -I$ENV{VLROOT} -L$ENV{VLROOT}/bin/win64/ ")
    set (CMAKE_SHARED_LINKER_FLAGS "-lgdi32")
    add_executable(ImageStitching 
                    CImg.h main.cpp Blend.h Blend.cpp Feature.cpp Feature.h FileReading.cpp FileReading.h
                   Interpolation.cpp Interpolation.h Match.cpp Match.h Projection.cpp Projection.h 
                   Stitching.cpp Stitching.h Warping.cpp Warping.h)
    configure_file(vl.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vl.dll COPYONLY)
    target_link_libraries(ImageStitching vl.lib)
endif(WIN32)

include_directories(.)
