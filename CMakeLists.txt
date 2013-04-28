cmake_minimum_required(VERSION 2.8)

project(EyeOfFly)

add_definitions( -DBOOST_ASIO_ENABLE_HANDLER_TRACKING )

add_executable(EyeOfFly fly.cc cam.cc dim.cc disp.cc framempool.cc streamer.cc camcon)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_NO_SYSTEM_PATH         ON)

find_package(FLTK                          REQUIRED)
find_package(OpenGL                        REQUIRED)
find_package(Boost  1.53 COMPONENTS system REQUIRED)

if(Boost_FOUND)
  include_directories  (${Boost_INCLUDE_DIRS}) 
  message("Boost libs: ${Boost_LIBRARIES}")
  target_link_libraries(EyeOfFly ${Boost_LIBRARIES})
else()
  message(FATAL_ERROR "boost should be installed")
endif()

set( USE_CXX11      "-std=c++0x -Wfatal-errors"       )
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${USE_CXX11} -flto" )

#include_directories(${PROJECT_SOURCE_DIR}/../lib)

target_link_libraries(EyeOfFly ${FLTK_LIBRARIES}  ${OPENGL_LIBRARIES} )
target_link_libraries(EyeOfFly PocoNet PocoFoundation  )
