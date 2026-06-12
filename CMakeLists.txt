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

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_executable(oxygenlink WIN32 
            src/main.cpp
            src/resources.rc
        )
        
        target_link_options(oxygenlink PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
    else()
        add_executable(oxygenlink 
            src/main.cpp
            src/resources.rc
        )
    endif()
else()
    add_executable(oxygenlink src/main.cpp)
endif()

target_link_libraries(oxygenlink PRIVATE lib)

if(WIN32)
    include(InstallRequiredSystemLibraries)
endif()

install(TARGETS oxygenlink DESTINATION ".")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/oxyconnect.jar" DESTINATION ".")


install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tools/" DESTINATION "tools")

if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ".")
endif() 


set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_target(copy_launcher_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/tools"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/tools" "${OUTPUT_DIR}/tools"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/src/oxyconnect.jar" "${OUTPUT_DIR}/oxyconnect.jar"
    COMMENT "Copying tools/ directory and oxyconnect.jar to the build output folder..."
    VERBATIM
)
add_dependencies(oxygenlink copy_launcher_assets)