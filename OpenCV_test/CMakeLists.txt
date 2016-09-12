cmake_minimum_required(VERSION 3.5)
project(OpenCV_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#include path
include_directories(/usr/local/include)

set(SOURCE_FILES main.cpp ImgProcessing.cpp ImgProcessing.h DegreofCircle.cpp DegreofCircle.h)
add_executable(OpenCV_test ${SOURCE_FILES})

# OpenCV3を利用する
set(OpenCV_DIR /usr/local/opt/opencv3/share/OpenCV)
find_package(OpenCV REQUIRED)

# OpenCV3のヘッダーパスを指定する
include_directories(${OpenCV_INCLUDE_DIRS})

# OpenCV3のライブラリを結合する
target_link_libraries(OpenCV_test ${OpenCV_LIBRARIES})