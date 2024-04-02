
add_executable(pngtest pngtest.c)
target_link_libraries(pngtest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

add_executable(bmptest bmptest.c)
target_link_libraries(bmptest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

add_executable(tgatest tgatest.c)
target_link_libraries(tgatest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

add_executable(pgmtest pgmtest.c)
target_link_libraries(pgmtest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

add_executable(jpegtest jpegtest.c)
target_link_libraries(jpegtest
  PRIVATE nadzuna
  PUBLIC nadzuna_compiler_flags)

# EOF
