CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(Lexical_analyzer)

file(GLOB_RECURSE HEADER_FILES "include/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_library(Lexer ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(Lexer PUBLIC include)
target_link_libraries(Lexer Regex)

install(TARGETS Lexer DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/Lexer)

