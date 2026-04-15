cmake_minimum_required(VERSION 3.20)

project(ComputationalFinanceCPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put all executables in one place
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build every .cpp file in the repo root as a separate executable
file(GLOB ROOT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")

foreach(SRC ${ROOT_CPP})
    get_filename_component(TARGET_NAME ${SRC} NAME_WE)
    add_executable(${TARGET_NAME} ${SRC})

    # Your repo root contains the Eigen/ folder, so this supports:
    #   #include <Eigen/Dense>
    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}
    )
endforeach()