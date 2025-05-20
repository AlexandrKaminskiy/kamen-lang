cmake_minimum_required(VERSION 3.10)
project(calc_project)

# Найти Bison и Flex
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Генерация из parser.y и lexer.l
BISON_TARGET(Parser
        parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp
        # УБРАНО: COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}"
)
FLEX_TARGET(Scanner
        lexer.l
        ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp
)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# Подключить директории для include
include_directories(
        ${CMAKE_SOURCE_DIR}           # для functions.h
        ${CMAKE_CURRENT_BINARY_DIR}   # для parser.tab.hpp
)

# Создание исполняемого файла
add_executable(calc
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
        syntax.cpp
        semantic.cpp
)

# Линковка с libfl
# Указать путь к установленной libfl
#link_directories(/opt/homebrew/opt/flex/lib)
#
## Указать путь к include, если нужно
#include_directories(/opt/homebrew/opt/flex/include)
#
## Линковка с libfl
#target_link_libraries(calc PRIVATE fl)

#include_directories(/opt/homebrew/opt/flex/lib)
#target_link_libraries(calc PRIVATE lfl)

# Стандарт C++
set_target_properties(calc PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
)

# Символы отладки
set(CMAKE_BUILD_TYPE Debug)
