cmake_minimum_required(VERSION 3.16)
project(regex_to_nfa C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(regex_to_nfa
    src/main.c
    src/tokenizer.c
    src/regex.c
    src/nfa.c
)

target_include_directories(regex_to_nfa PRIVATE src)

# Pruebas unitarias. Cada componente se compila con lo minimo que necesita
# para poder probarlo por separado. Se corren con ctest.
enable_testing()

add_executable(test_tokenizer tests/test_tokenizer.c src/tokenizer.c)
target_include_directories(test_tokenizer PRIVATE src)
add_test(NAME tokenizer COMMAND test_tokenizer)

add_executable(test_regex tests/test_regex.c src/regex.c src/tokenizer.c)
target_include_directories(test_regex PRIVATE src)
add_test(NAME regex COMMAND test_regex)
