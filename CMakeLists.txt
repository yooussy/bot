cmake_minimum_required(VERSION 3.10)

project(bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(bot src/main.cpp)

find_package(Poco REQUIRED COMPONENTS Foundation Net JSON)
target_link_libraries(bot PRIVATE PocoNet PocoNetSSL PocoFoundation PocoJSON PocoUtil)

if(NOT Poco_FOUND)
    message(WARNING "POCO library not found")
else()
    message(STATUS "Found POCO version ${Poco_VERSION}")
endif()
