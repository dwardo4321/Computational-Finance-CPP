cmake_minimum_required(VERSION 3.20)

project(ComputationalFinanceCPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Header files
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    C:/msys64/ucrt64/include/eigen3
    C:/msys64/ucrt64/include
)

# Automatically collect all implementation files in src/
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Automatically collect all runnable files in app/
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/app/*.cpp
)

# Create one executable for each .cpp file in app/
foreach(app_file ${APP_SOURCES})

    get_filename_component(app_name ${app_file} NAME_WE)

    add_executable(${app_name}
        ${app_file}
        ${PROJECT_SOURCES}
    )

    target_include_directories(${app_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        C:/msys64/ucrt64/include/eigen3
        C:/msys64/ucrt64/include
    )

endforeach()