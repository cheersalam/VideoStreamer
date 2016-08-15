# Mohammad Alam
# cheersalam@gmail.com
# cmake file

cmake_minimum_required (VERSION 3.0)
set(PROJECT_NAME "BebopVideoStreamer")
project (${PROJECT_NAME})



set(CMAKE_VERBOSE_MAKEFILE on)

#------------------------------------------------------------------------------
# Compiler Flags options 
#------------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" CACHE PATH "Output directory for applications" )
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    MESSAGE(" Configuring flags for GNU:")
    set(VS_C_FLAGS "-Wall -Wextra -W ")
    set(VS_C_FLAGS_RELEASE "-O2")
    set(VS_C_FLAGS_DEBUG "-g3 -O0")
    set(VS_CXX_FLAGS "-Wall -Wextra -W")
    set(VS_CXX_FLAGS_RELEASE "-O2")
    set(VS_CXX_FLAGS_DEBUG "-g3 -O0")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

#------------------------------------------------------------------------------
#Settings MSVC Macros
#------------------------------------------------------------------------------
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    MESSAGE(" Configuring flags for MSVC:")
    set(VS_C_FLAGS "/W4 /WX /EHsc")
    set(VS_C_FLAGS_RELEASE "-O2")
    set(VS_C_FLAGS_DEBUG "-g3 -O0")
    set(VS_CXX_FLAGS "/W4 /WX /EHsc")
    set(VS_CXX_FLAGS_RELEASE "-O2")
    set(VS_CXX_FLAGS_DEBUG "-g3 -O0")
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

#only append flags to existing flags
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${VS_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} ${VS_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} ${VS_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${VS_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${VS_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${VS_CXX_FLAGS_DEBUG}")

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    CACHE STRING "Choose the type of build: None Debug Release"
    FORCE)

set(LIB_INSTALL_DIR lib)
include_directories(include/)
link_directories(${PROJECT_BINARY_DIR}/lib/)

#------------------------------------------------------------------------------
# Project Settings: path for lib, bin and include folder
#------------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" CACHE PATH "Output directory for applications" )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" CACHE PATH "Output Directory for Libs")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/include")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#------------------------------------------------------------------------------
# Project Settings: Debug or Release 
#------------------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" CACHE PATH "Output directory for applications" )
get_filename_component(COMPILE_TYPE ${CMAKE_CURRENT_BINARY_DIR} NAME)
if( ${COMPILE_TYPE} STREQUAL "debug" OR 
    ${COMPILE_TYPE} STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE "Debug")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

#------------------------------------------------------------------------------
# External Project - cJSON - json parser library
#------------------------------------------------------------------------------
include(ExternalProject)
ExternalProject_Add(
                    cJSON
                    GIT_REPOSITORY https://github.com/DaveGamble/cJSON
                    GIT_TAG master
                    UPDATE_COMMAND git pull
                    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/cJSON
                    BUILD_IN_SOURCE 1
                    BUILD_ALWAYS 0
                    CONFIGURE_COMMAND ""
                    BUILD_COMMAND make
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ./cJSON.h "${PROJECT_BINARY_DIR}/include"
                    && ${CMAKE_COMMAND} -E copy ./libcjson.a "${PROJECT_BINARY_DIR}/lib")

ExternalProject_Add(
                    VideoContainerGenerator
                    GIT_REPOSITORY https://github.com/cheersalam/VideoContainerGenerator
                    GIT_TAG master
                    UPDATE_COMMAND ""
                    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/VideoContainerGenerator
                    BUILD_IN_SOURCE 1
                    BUILD_ALWAYS 0
                    INSTALL_DIR ${PROJECT_BINARY_DIR}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/thirdparty/VideoContainerGenerator/include ${PROJECT_BINARY_DIR}/include/ &&
                   ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/thirdparty/VideoContainerGenerator/lib ${PROJECT_BINARY_DIR}/lib/  &&
                   ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/thirdparty/VideoContainerGenerator/bin ${PROJECT_BINARY_DIR}/bin/)

#------------------------------------------------------------------------------
# Adding cJSON lib
#------------------------------------------------------------------------------
set(JSON_PARSER_LIB libcjson.a -lm)

#------------------------------------------------------------------------------
# Adding FFMPEG lib
#------------------------------------------------------------------------------
set(FFMPEG_LIBS avcodec avdevice avfilter avformat avutil swresample swscale)

#------------------------------------------------------------------------------
# Adding socklib lib
#------------------------------------------------------------------------------
set(SOCK_LIB libsocklib.a)

#------------------------------------------------------------------------------
# Adding VideoContainerGenerator lib
#------------------------------------------------------------------------------
set(VCG_LIB libffmpegContainerGenerator.a)

#------------------------------------------------------------------------------
# Add all servers lib dependecies here. (/trunk/src/servers) 
#------------------------------------------------------------------------------
set(PROJECT_LIBS ${JSON_PARSER_LIB} ${FFMPEG_LIBS} ${SOCK_LIB} ${VCG_LIB} -lSDL)

#------------------------------------------------------------------------------
# Adding colors 
#------------------------------------------------------------------------------
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

#------------------------------------------------------------------------------
# Sub Directories 
#------------------------------------------------------------------------------
add_subdirectory (socklib) 
add_subdirectory (videostreamer) 

#------------------------------------------------------------------------------
# Configuration Summary 
#------------------------------------------------------------------------------
MESSAGE("")
MESSAGE(" Platform:")
MESSAGE("   Host:             "  ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR})
MESSAGE("   CMake Version:    "  ${CMAKE_VERSION})
if(MSVC)
    MESSAGE("   MSVC:             "         ${MSVC_VERSION})   
endif()

MESSAGE("   C flags           "  ${CMAKE_C_FLAGS})
MESSAGE("   C flags (Release) "  ${CMAKE_C_FLAGS_RELEASE})
MESSAGE("   C flags (Debug)   "  ${CMAKE_C_FLAGS_DEBUG})
MESSAGE("   Build Type        "  ${CMAKE_BUILD_TYPE})



