cmake_minimum_required(VERSION 3.15)

project(BuildYourOwnLisp)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(parsing parsing.c constructors.c lval.h mpc.c)

target_link_libraries(parsing PUBLIC edit)
