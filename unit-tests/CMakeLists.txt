##### GOOGLETEST SETUP VARIABLES #####

cmake_minimum_required(VERSION 3.14)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/4fed5f285030085b096c930ff03e42c7814739b2.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

##### PROJECT VARIABLES #####

set(This webserv)

project(${This} CXX)

enable_testing()

set(Sources
	#Insert .cpp files that will be compiled into a `.a` library
)

set(Headers
	#Insert .hpp files that will be needed for compilation
)

add_library(${This} ${Sources} ${Headers})

##### SUBDIRECTORIES CONTAINING THE UNIT TESTS #####

#add_subdirectory(InputTests)
