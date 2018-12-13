# rtmp library 

cmake_minimum_required (VERSION 3.5.1)
project (rtmp-module)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.66 REQUIRED COMPONENTS system coroutine thread context regex filesystem log_setup log program_options REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories (
  SYSTEM ${Boost_INCLUDE_DIRS}
  SYSTEM ${LIBXML2_INCLUDE_DIR}
  /usr/inlcude
  rtmps/src
  rtmps/src/common
  rtmps/src/logic
  rtmps/src/network
  rtmps/src/protocol
  rtmps/src/protocol/cmd_message
  rtmps/src/protocol/control_message
  rtmps/src/protocol/data_message
  rtmps/src/protocol/media_message
  rapidjson
  streamer
  src
  .
)
add_compile_options(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1 -g)

add_subdirectory(rtmps)

set(SRCS 
  src/main.cpp
)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME}
  ${Boost_LIBRARIES}
  rtmps
  ssl
  crypto
  pthread
  dl
  rt
)
#add_subdirectory(example EXCLUDE_FROM_ALL)
#add_subdirectory(tool EXCLUDE_FROM_ALL)
