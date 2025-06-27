cmake_minimum_required (VERSION 3.15...4.0)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (ColorCorrectionMatrixSolver VERSION 1.0 LANGUAGES CXX)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

find_package(Ceres REQUIRED)

set(SOURCES
  src/main.cpp
  src/image_data.hpp
  src/image_file_handler.hpp
  src/image_file_handler.cpp
  src/color_correction_matrix_solver.hpp
  src/color_correction_matrix_solver.cpp
)

add_executable(ColorCorrectionMatrixSolver ${SOURCES})

target_include_directories(ColorCorrectionMatrixSolver 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(ColorCorrectionMatrixSolver 
  PRIVATE 
    project_options
    Ceres::ceres
)