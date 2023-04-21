cmake_minimum_required(VERSION 3.8)

project(main)
include_directories(include)

add_library(lib1 SHARED src/lib1.c)
add_library(lib2 SHARED src/lib2.c)

add_executable(main1 src/main1.c include/lib.h)
add_executable(main2 src/main1.c include/lib.h)
target_link_libraries(main1 lib1)
target_link_libraries(main2 lib2)

add_executable(main src/main2.c)
target_link_libraries(main ${CMAKE_DL_LIBS})
