
set(library_sources
  bitmapimg.c
  bitmapimg.h
  color.c
  color.h
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

add_library(nadzuna STATIC ${library_sources})

if(MATH_LIBRARY)
  target_link_libraries(nadzuna PUBLIC PNG::PNG m)
else(MATH_LIBRARY)
  target_link_libraries(nadzuna PUBLIC PNG::PNG)
endif(MATH_LIBRARY)

target_include_directories(nadzuna PUBLIC ${PROJECT_BINARY_DIR})

add_executable(pngtest pngtest.c)
target_link_libraries(pngtest PRIVATE nadzuna)

add_executable(bmptest bmptest.c)
target_link_libraries(bmptest PRIVATE nadzuna)

# EOF
