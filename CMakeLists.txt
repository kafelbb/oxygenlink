cmake_minimum_required(VERSION 3.31)
project(oxygenlink LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_library(lib STATIC
    src/extadb.cpp
    src/logger.h
    src/extadb.h
    src/ui.cpp
    src/pfd.h
    src/ui.h
    src/icongrabber.h
    src/icongrabber.cpp
)

target_include_directories(lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
) 

target_link_libraries(lib PUBLIC 
    sfml-graphics
    sfml-window
    sfml-system
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(oxygenlink WIN32 
        src/main.cpp
        src/resources.rc
    )
    
    target_link_libraries(oxygenlink PRIVATE 
        lib
    )
    
    target_link_options(oxygenlink PRIVATE "/SUBSYSTEM:WINDOWS")
else()
    add_executable(oxygenlink 
        src/main.cpp
        src/resources.rc
    )
    target_link_libraries(oxygenlink PRIVATE lib)
endif()


file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools" TOOLS_SRC_DIR)
file(TO_CMAKE_PATH "$$<TARGET_FILE_DIR:oxygenlink>/tools" TOOLS_DST_DIR)

add_custom_command(
    TARGET oxygenlink POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${CMAKE_CURRENT_SOURCE_DIR}/tools 
            $<TARGET_FILE_DIR:oxygenlink>/tools
    COMMENT "cloning /tools dir to build directory"
)