cmake_minimum_required(VERSION 2.8)

project(EyeOfFly)

find_package(FLTK                          REQUIRED)
find_package(OpenGL                        REQUIRED)
find_package(Boost  1.53 COMPONENTS system REQUIRED)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) 
else()
  message(FATAL_ERROR "boost should be installed")
endif()

set( USE_CXX11      "-std=c++0x -Wfatal-errors"       )
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${USE_CXX11}" )

#include_directories(${PROJECT_SOURCE_DIR}/../lib)

add_executable(EyeOfFly fly.cc cam.cc dim.cc disp.cc framempool.cc streamer.cc)

target_link_libraries(EyeOfFly ${BOOST_LIBRARIES}                     )
target_link_libraries(EyeOfFly ${FLTK_LIBRARIES}  ${OPENGL_LIBRARIES} )
target_link_libraries(EyeOfFly ${POCO_LIBS}         PocoNet PocoFoundation  )
