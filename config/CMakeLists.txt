CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(config)

include(ExternalProject)

find_package(Git REQUIRED)

ExternalProject_Add(RegularExpression
    GIT_REPOSITORY "https://github.com/krystianAndrzejewski/regular_expression"
    SOURCE_DIR ${EXTERNAL_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    )
    