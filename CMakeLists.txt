cmake_minimum_required(VERSION 3.15)

project(Nadzuna
  VERSION 0.1
  LANGUAGES C)

find_library(MATH_LIBRARY m)

find_package(PNG)
if(PNG_FOUND)
  set(HAVE_LIBPNG 1)
endif(PNG_FOUND)

find_package(JPEG)
if (JPEG_FOUND)
  set(HAVE_LIBJPEG 1)
endif(JPEG_FOUND)

configure_file(config.h.in config.h)

add_library(nadzuna_compiler_flags INTERFACE)
target_compile_features(nadzuna_compiler_flags INTERFACE c_std_99)
target_compile_options(nadzuna_compiler_flags INTERFACE
  $<$<C_COMPILER_ID:MSVC>:/W4>
  $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
)

add_subdirectory(lib)
add_subdirectory(bin)

# EOF
