#
# CMake build script for PettyUtil.
#

# Is debug option.
option(IS_DEBUG "Is debug" OFF)

# Without test option.
option(WITHOUT_TEST "Without test" OFF)

# CMake project.
project(thincautility CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

if(IS_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

# Build tool option.
if(MSVC)
  foreach(flag_var
          CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
          CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endif()
  endforeach()
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -D_DEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -D_DEBUG")
else()
  message(SEND_ERROR "You are on an unsupported platform!")
endif()

# Include header.
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR})

# Target source.
set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/base64.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduled_executor.cpp)
if(MSVC)
  set(SRC_FILES ${SRC_FILES}
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/atomic.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/cpu_timer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/dynamic_library.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/mutex.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/semaphore.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/thread.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-windows/wstring_utility.cpp)
elseif(UNIX)
  set(SRC_FILES ${SRC_FILES}
      ${CMAKE_CURRENT_SOURCE_DIR}/src-unix/atomic.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-unix/cpu_timer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-unix/dynamic_library.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-unix/mutex.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-unix/semaphore.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src-unix/thread.cpp)
endif()

# Build static link library.
add_library(thincautility STATIC ${SRC_FILES})

# Build tests.
if(NOT WITHOUT_TEST)
  enable_testing()
  add_subdirectory(test)
endif()
