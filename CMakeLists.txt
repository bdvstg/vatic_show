cmake_minimum_required(VERSION 3.0)

# please append -G"Visual Studio 12" with cmake if ver of OpenCV smaller 3

project(vatic_show)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Cereal REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5
        COMPONENTS
            Gui
            LinguistTools
            Widgets
        REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${Cereal_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)
link_directories(lib)

#add_compile_options(-std=c++11)

if(WIN32)
    add_compile_options(/wd4996)
endif()

#qt5_add_translation(TRANSLATIONS *.ts)
file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)
add_executable(vatic_show ${SOURCES} ${HEADERS})# ${TRANSLATIONS})

target_link_libraries(vatic_show
    ${OpenCV_LIBS}
    iconv
    zdll
    libxml2
    User32
    Qt5::Gui
    Qt5::Widgets)
