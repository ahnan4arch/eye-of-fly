cmake_minimum_required(VERSION 2.8)

project(EyeOfFly)

add_definitions( -DNDEBUG )
# add_definitions( -DBOOST_ASIO_ENABLE_HANDLER_TRACKING -DDEBUG )

add_executable(EyeOfFly stat view cam dim disp framempool streamer jpegd camcon rec init)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_NO_SYSTEM_PATH         ON)

set(FLTK_SKIP_FORMS  ON)
set(FLTK_SKIP_IMAGES ON)

find_package(FLTK                          REQUIRED FLTK_SKIP_FORMS FLTK_SKIP_IMAGES)
find_package(OpenGL                        REQUIRED)
find_package(Boost  1.53 COMPONENTS system REQUIRED)

if(Boost_FOUND)
  include_directories  (${Boost_INCLUDE_DIRS}) 
#  message("Boost libs: ${Boost_LIBRARIES}")
  target_link_libraries(EyeOfFly ${Boost_LIBRARIES})
else()
  message(FATAL_ERROR "boost should be installed")
endif()

set( USE_CXX11        "-std=c++0x -Wfatal-errors"                      )
#set( CMAKE_CXX_FLAGS  "-Wall -g ${CMAKE_CXX_FLAGS} ${USE_CXX11} -flto" )
#set( CMAKE_CXX_FLAGS  "-Wall -g ${CMAKE_CXX_FLAGS} ${USE_CXX11}" )
set( CMAKE_CXX_FLAGS  "-Wall -O3 ${CMAKE_CXX_FLAGS} ${USE_CXX11}" )

#include_directories(${PROJECT_SOURCE_DIR}/../lib)
#message("Libs: ${FLTK_LIBRARIES}  ${OPENGL_LIBRARIES}")

target_link_libraries(EyeOfFly turbojpeg)
target_link_libraries(EyeOfFly ${FLTK_LIBRARIES}  ${OPENGL_LIBRARIES} )
target_link_libraries(EyeOfFly PocoNet PocoFoundation  )
