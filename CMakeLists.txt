CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(lexical_analysis_sample)

set(EXTERNAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

set(ARGS_TO_GENERATOR -DEXTERNAL_SOURCE_DIR:PATH=${EXTERNAL_SOURCE_DIR})
unset(EIGEN_DIR CACHE)

set(GENERATOR_BINARY_DIR ${CMAKE_BINARY_DIR}/config)

file(MAKE_DIRECTORY "${GENERATOR_BINARY_DIR}")

execute_process(COMMAND "${CMAKE_COMMAND}" -H${CMAKE_CURRENT_SOURCE_DIR}/config -B${GENERATOR_BINARY_DIR} ${ARGS_TO_GENERATOR}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config" )

execute_process(COMMAND "${CMAKE_COMMAND}" --build ${GENERATOR_BINARY_DIR}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config" )

add_subdirectory(${EXTERNAL_SOURCE_DIR})
add_subdirectory(Lexer)

add_executable(lexical_analysis_sample main.cpp)
target_link_libraries(lexical_analysis_sample Lexer)

install(TARGETS lexical_analysis_sample DESTINATION bin)

