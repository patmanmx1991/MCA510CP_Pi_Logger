cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/ -L/usr/local/lib/ -std=c++11 -Wall -g -O3 -I/usr/local/include/hidapi -lstdc++ -lhidapi-hidraw -lwiringPi -lwiringPiDev -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
project (USBGAMMADAQ)
set (SOURCE src/CInterface.cpp src/command.hpp)
add_library(USBGAMMADAQ SHARED ${SOURCE})
include_directories(src)

target_link_libraries(USBGAMMADAQ -L/usr/local/lib/ -lhidapi-hidraw -lwiringPi)

add_executable(connect ${PROJECT_SOURCE_DIR}/src/connect.cpp)
target_link_libraries(connect USBGAMMADAQ -L/usr/local/lib/ -lhidapi-hidraw -lwiringPi) 

add_executable(runser ${PROJECT_SOURCE_DIR}/src/serial.cpp)
target_link_libraries(runser USBGAMMADAQ -L/usr/local/lib/ -lhidapi-hidraw -lwiringPi) 
