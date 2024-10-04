project(ifj2024)

add_executable(ifj2024 src/main.c)

add_library(errors SHARED    src/utils/errors.c)
add_library(lexeme SHARED    src/utils/lexeme.c)
# add_library(scanner SHARED  src/scanner/scanner.c)

target_link_libraries(ifj2024 errors lexeme)
