
set(library_sources
  nadzuna.h
  image.c
  color.c
  color.h
  color_inline.h
  common.h
  error.c
  error.h
  file_io.c
  file_io.h
  pgmfile.c
  pngfile.c
  pngfile.h
  tgafile.c
  tgafile.h
  bmpfile.c
  bmpfile.h
)

if(WIN32)
  list(APPEND library_sources
    ${CMAKE_CURRENT_BINARY_DIR}/../windows/nadzuna.rc)
endif(WIN32)

add_library(nadzuna SHARED ${library_sources})

# Set the symbol defined when compiled as a shared library.
set_target_properties(nadzuna PROPERTIES DEFINE_SYMBOL NADZUNA_BUILD)
set_target_properties(nadzuna PROPERTIES C_VISIBILITY_PRESET hidden)

target_link_libraries(nadzuna PUBLIC PNG::PNG)

if(MATH_LIBRARY)
  target_link_libraries(nadzuna PUBLIC m)
endif(MATH_LIBRARY)
target_link_libraries(nadzuna PUBLIC nadzuna_compiler_flags)

target_include_directories(nadzuna PUBLIC ${PROJECT_SOURCE_DIR}/lib)
target_include_directories(nadzuna PUBLIC ${PROJECT_BINARY_DIR})

# EOF
