cmake_minimum_required(VERSION 3.14)

project(mem01 LANGUAGES CUDA CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(TBB REQUIRED)
find_package(CUDA REQUIRED)

add_definitions( -DHAVE_CUDA
                 -DHAVE_BEAMFORMER
                 -DHAVE_CUFFT
                 -DHAVE_DEVICE_METAIMAGE_OUTPUT
                 -DHAVE_BEAMFORMER_MINIMUM_VARIANCE
                 -DHAVE_DEVICE_ULTRASOUND_SIM
                 -DHAVE_CUDA_CUBLAS
                 -DHAVE_DEVICE_TRACKING_SIM )

include_directories(${CUDA_INCLUDE_DIRS}
                 common
                 utilities
                 memory
                 ../include
             )

link_directories(~
                ../libs
             )


file(GLOB_RECURSE SRC_LIST
                *.h
                *.cpp
                *.ui)

add_executable(mem01
  ${SRC_LIST}
)

target_link_libraries(mem01 Qt${QT_VERSION_MAJOR}::Core
                glog
                pthread
                tbb
                ${CUDA_LIBRARIES}
            )
