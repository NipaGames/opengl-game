cmake_minimum_required(VERSION 3.10)
project(OpenGLProject VERSION 0.5)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")

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
FILE(GLOB ThirdPartySources LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/include/*)
foreach(Dir ${ThirdPartySources})
    if(IS_DIRECTORY ${Dir})
        include_directories(${Dir})
    endif()
endforeach()

link_directories(${CMAKE_SOURCE_DIR}/bin)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
link_directories(${CMAKE_SOURCE_DIR}/lib)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

FILE(GLOB_RECURSE Sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER Sources EXCLUDE REGEX ".*thirdparty/[^\\.]*\\.cpp$")
add_executable(OpenGLProject ${Sources})
target_compile_definitions(OpenGLProject PRIVATE VERSION_MAJ=${PROJECT_VERSION_MAJOR} VERSION_MIN=${PROJECT_VERSION_MINOR})
string (REPLACE ";" "\n\t- " SourcesFormatted "\n\t- ${Sources}")
message("All project sources: ${SourcesFormatted}")

### ALL LIBRARIES ###
target_link_libraries(OpenGLProject opengl32)
target_link_libraries(OpenGLProject glfw3)
target_link_libraries(OpenGLProject glew32)
target_link_libraries(OpenGLProject assimp-vc143-mt)
target_link_libraries(OpenGLProject freetype)
target_link_libraries(OpenGLProject glm::glm)
target_link_libraries(OpenGLProject spdlog::spdlog)
target_link_libraries(OpenGLProject fmt)
target_link_libraries(OpenGLProject BulletDynamics)
target_link_libraries(OpenGLProject BulletCollision)
target_link_libraries(OpenGLProject LinearMath)