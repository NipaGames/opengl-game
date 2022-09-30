cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

FILE(GLOB ThirdPartySources LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/src/thirdparty/*)
foreach(Dir ${ThirdPartySources})
    if(IS_DIRECTORY ${Dir})
        add_subdirectory(${Dir})
    endif()
endforeach()
string (REPLACE ";" "/\n\t- " ThirdPartySourcesFormatted "\n\t- ${ThirdPartySources}/")
message("All third-party source directories: ${ThirdPartySourcesFormatted}")

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/bin)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
link_directories(${CMAKE_SOURCE_DIR}/lib)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

FILE(GLOB_RECURSE Sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER Sources EXCLUDE REGEX ".*thirdparty/[^\\.]*\\.cpp$")
add_executable(OpenGLProject ${Sources})
string (REPLACE ";" "\n\t- " SourcesFormatted "\n\t- ${Sources}")
message("All project sources: ${SourcesFormatted}")

### ALL LIBRARIES ###
target_link_libraries(OpenGLProject opengl32)
target_link_libraries(OpenGLProject glfw3)
target_link_libraries(OpenGLProject glew32)
target_link_libraries(OpenGLProject glm::glm)
target_link_libraries(OpenGLProject spdlog::spdlog)