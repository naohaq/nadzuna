cmake_minimum_required(VERSION 3.10)

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

add_subdirectory(src)

# EOF
