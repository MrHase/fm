project(fm)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

IF(EXISTS "/usr/share/fonts/TTF/FreeSans.ttf")
ELSE()
MESSAGE("Please install ttf-freefont")
ENDIF()

add_definitions(-std=c++11)
set(CMAKE_BUILD_TYPE Debug)
FIND_PACKAGE(SDL)
#FIND_PACKAGE(JsonCpp)
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
LINK_LIBRARIES(${SDLMAIN_LIBRARIES} ${SDL_LIBRARIES})

add_executable(${PROJECT_NAME} ${SRC_LIST})

#target_link_libraries(${PROJECT_NAME} SDL SDL_ttf pthread lua5.2)
target_link_libraries(${PROJECT_NAME} SDL SDL_ttf pthread lua jsoncpp /home/mrhase/dev/xmp/libxmplib.so /home/mrhase/dev/xmp/libtinyxml.so)
