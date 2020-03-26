cmake_minimum_required(VERSION 3.15)

project(BuildYourOwnLisp)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(main main.c parsing.c lenv.c lval.c mpc.c builtins.c)

target_link_libraries(main PUBLIC edit)
