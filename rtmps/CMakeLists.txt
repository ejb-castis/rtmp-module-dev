# rtmp library 

cmake_minimum_required (VERSION 3.5.1)
project (rtmps)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.66 REQUIRED COMPONENTS system coroutine thread context regex filesystem log_setup log program_options REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories (
  SYSTEM ${Boost_INCLUDE_DIRS}
  SYSTEM ${LIBXML2_INCLUDE_DIR}
  /usr/inlcude
  common
  logic
  network
  protocol
  src
  .
)

add_compile_options(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1 -g)

add_subdirectory(src)

#add_subdirectory(example EXCLUDE_FROM_ALL)
#add_subdirectory(tool EXCLUDE_FROM_ALL)
