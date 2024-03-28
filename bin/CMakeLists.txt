
add_executable(pngtest pngtest.c)
target_link_libraries(pngtest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

add_executable(bmptest bmptest.c)
target_link_libraries(bmptest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

# EOF
