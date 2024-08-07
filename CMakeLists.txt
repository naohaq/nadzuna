cmake_minimum_required(VERSION 3.15)

project(Nadzuna
  VERSION 0.1
  LANGUAGES C)
set(COPYRIGHT_YEAR "2024")

find_library(MATH_LIBRARY m)

find_package(PNG)
if(PNG_FOUND)
  set(HAVE_LIBPNG 1)
endif(PNG_FOUND)

find_package(JPEG)
if(JPEG_FOUND)
  set(HAVE_LIBJPEG 1)
endif(JPEG_FOUND)

configure_file(config.h.in config.h)

if(WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lib/win/nadzuna.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/windows/nadzuna.rc @ONLY)
endif(WIN32)

add_library(nadzuna_compiler_flags INTERFACE)
target_compile_features(nadzuna_compiler_flags INTERFACE c_std_99)
target_compile_options(nadzuna_compiler_flags INTERFACE
  $<$<C_COMPILER_ID:MSVC>:/W4>
  $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
)

add_subdirectory(lib)
add_subdirectory(bin)

# EOF
