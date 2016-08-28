#------------------------------------------------------------------------------
# Mohammad Alam
# cheersalam@gmail.com
#------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8)

#Use folder name as project name
get_filename_component(COMP_NAME ${CMAKE_CURRENT_BINARY_DIR} NAME)
project (${COMP_NAME})
MESSAGE("COMP_NAME " ${COMP_NAME})


#Glob all .c files present in folder
FILE(GLOB ALL_C_FILES *.c)

#cmake option for generating a static library
add_library (${COMP_NAME} STATIC ${ALL_C_FILES})

# Glob all .h files and move to top directory include folder
# if you dont want all filed to be moved then manually add
# name of the files here
FILE(GLOB ALL_H_FILES *.h)

#Copy all header files to include folder
foreach(EACH_H_FILE ${ALL_H_FILES})
  add_custom_command(TARGET 
                    ${COMP_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy ${EACH_H_FILE} 
                    "${CMAKE_BINARY_DIR}/include/")
endforeach()

#------------------------------------------------------------------------------
# Generic file ends here. Add project related settings below
#------------------------------------------------------------------------------


#For header files
include_directories("${CMAKE_BINARY_DIR}/include")


