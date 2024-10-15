cmake_minimum_required(VERSION 3.10)
project(ifj2024)
set(CMAKE_C_STANDARD 99)
include (CTest)

set(VALGRIND_OK ".*All heap blocks were freed -- no leaks are possible.*")
set(CMAKE_BUILD_TYPE Debug)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(UTILS ${SRC}/utils)
set(SCANNER ${SRC}/scanner)
add_executable(ifj2024 src/main.c)

add_library(errors SHARED    	 ${UTILS}/errors.c)
add_library(token SHARED    	 ${UTILS}/token.c)
add_library(keyword SHARED 	 	 ${UTILS}/keywords.c)
add_library(dll SHARED    	     ${UTILS}/dll.c)
add_library(scanner SHARED    	 ${SCANNER}/scanner.c)
add_library(token_dll SHARED 	 ${UTILS}/token_dll.c)

target_link_libraries(scanner token keyword token_dll)
target_link_libraries(ifj2024 errors token scanner)
enable_testing()


# tests
add_executable(test_dll_1 ${TEST}/dll/DLL_test_1.c)
add_executable(test_dll_2 ${TEST}/dll/DLL_test_2.c)
add_executable(test_dll_3 ${TEST}/dll/DLL_test_3.c)
add_executable(test_dll_4 ${TEST}/dll/DLL_test_4.c)
add_executable(test_scan_1 ${TEST}/scanner/scan_1.c)


target_link_libraries(test_dll_4 token dll keyword)
target_link_libraries(test_scan_1 token dll keyword scanner)

# test covering initialization, push_front and clear in DLL macro
add_test(NAME "dll-init" COMMAND test_dll_1)
set_property (TEST "dll-init"
              PROPERTY PASS_REGULAR_EXPRESSION ".*11 10 9 8 7 42 6 5 4 3 2 1.*")

add_test(NAME "dll-init-valgrind" COMMAND valgrind ./test_dll_1)
set_property (TEST "dll-init-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering pushes, pops and deletion on specific index in DLL macro
add_test(NAME "dll-push-pop-delete" COMMAND test_dll_2)
set_property (TEST "dll-push-pop-delete"
              PROPERTY PASS_REGULAR_EXPRESSION "1 2 3 4 5 42 6 7 8 9 10.*
.*2 3 4 42 6 7 8 9.*")

add_test(NAME "dll-push-pop-delete-valgrind" COMMAND valgrind ./test_dll_2)
set_property (TEST "dll-push-pop-delete-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering DLL with strings
add_test(NAME "dll-string" COMMAND test_dll_3)
set_property (TEST "dll-string"
              PROPERTY PASS_REGULAR_EXPRESSION "Lorem ipsum dolor sit amet incididunt consectetur adipiscing elit sed do eiusmod tempor")

add_test(NAME "dll-string-valgrind" COMMAND valgrind ./test_dll_2)
set_property (TEST "dll-string-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})

# test covering DLL with custom structure
add_test(NAME "dll-token" COMMAND test_dll_4)
set_property (TEST "dll-token"
              PROPERTY PASS_REGULAR_EXPRESSION ".*const else fn if i32 while f64 null pub return u8 void while.*")

add_test(NAME "dll-token-valgrind" COMMAND valgrind ./test_dll_4)
set_property (TEST "dll-token-valgrind"
			PROPERTY PASS_REGULAR_EXPRESSION ${VALGRIND_OK})
