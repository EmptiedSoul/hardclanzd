cmake_minimum_required(VERSION 3.13.4)
project(hardclanzd)
add_executable(hardclanzd hardclanzd.c)
set(GCC_FLAGS -O3 -Wall -Wextra -pedantic -Werror -DNDEBUG)
target_compile_options(hardclanzd PUBLIC ${GCC_FLAGS})
set_target_properties(hardclanzd PROPERTIES PUBLIC_HEADER service)
install(TARGETS hardclanzd DESTINATION "sbin"
	     PUBLIC_HEADER DESTINATION "sbin"
	)
