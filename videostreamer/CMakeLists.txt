
cmake_minimum_required (VERSION 2.8)

#Use folder name as project name
set(COMP_NAME "BebopVideoStream")
project (${COMP_NAME})
MESSAGE("COMP_NAME " ${COMP_NAME})

#Add project version number

FILE(GLOB ALL_C_FILES *.c)

#Include Folder
include_directories ("${CMAKE_BINARY_DIR}/include")


#------------------------------------------------------------------------------
# Generic file ends here. Add project related settings in section below
#------------------------------------------------------------------------------

#list(REMOVE_ITEM PROJECT_LIBS fcgi cJSON )

#------------------------------------------------------------------------------
# Generating executable
#------------------------------------------------------------------------------
add_executable (${COMP_NAME} ${ALL_C_FILES})
#add_dependencies(${COMP_NAME} jsmn)
set_target_properties(${COMP_NAME} PROPERTIES 
                      INSTALL_RPATH "/usr/local/lib")
set_target_properties(${COMP_NAME} PROPERTIES 
                      BUILD_WITH_INSTALL_RPATH true
                      ENABLE_EXPORTS ON)
target_link_libraries (${COMP_NAME} ${JSON_PARSER_LIB})


