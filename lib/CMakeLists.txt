
set(library_sources
  bitmapimg.c
  bitmapimg.h
  color.c
  color.h
  color_inline.h
  common.h
  error.c
  error.h
  file_io.c
  file_io.h
  pgmfile.c
  pgmfile.h
  pngfile.c
  pngfile.h
  tgafile.c
  tgafile.h
  bmpfile.c
  bmpfile.h
)

add_library(nadzuna SHARED ${library_sources})

# Set the symbol defined when compiled as a shared library.
set_target_properties(nadzuna PROPERTIES DEFINE_SYMBOL NADZUNA_EXPORTS)

target_link_libraries(nadzuna PUBLIC PNG::PNG)

if(MATH_LIBRARY)
  target_link_libraries(nadzuna PUBLIC m)
endif(MATH_LIBRARY)
target_link_libraries(nadzuna PUBLIC nadzuna_compiler_flags)

target_include_directories(nadzuna PUBLIC ${PROJECT_SOURCE_DIR}/lib)
target_include_directories(nadzuna PUBLIC ${PROJECT_BINARY_DIR})

# EOF
