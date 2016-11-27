cmake_minimum_required(VERSION 3.0)

# please append -G"Visual Studio 12" with cmake if ver of OpenCV smaller 3

project(vatic_show)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)
link_directories(lib)

#add_compile_options(-std=c++11)

if(WIN32)
    add_compile_options(/wd4996)
endif()

add_executable(vatic_show "*.cpp")

target_link_libraries(vatic_show
   ${OpenCV_LIBS}
   iconv
   zdll
   libxml2
   User32)
