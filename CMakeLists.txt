cmake_minimum_required(VERSION 3.15.2)

project(mikrotik-ssh-piano C CXX)

IF(MSVC)
  set(PROJECT_NAME "${PROJECT_NAME}-win")
ELSEIF(APPLE)
  set(PROJECT_NAME "${PROJECT_NAME}-osx")
ELSEIF(UNIX)
  set(PROJECT_NAME "${PROJECT_NAME}-linux")
ENDIF()

set(REQUIRED_LIBSSH_VERSION 1.10.0)
IF(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MT")
ELSE()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libstdc++")
ENDIF(MSVC)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
ELSE()
  message(" ! ! ! Unknown build type.")
ENDIF()

message("CMAKEFLAGS DUMP: ${CMAKE_CXX_FLAGS}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES libssh2/${REQUIRED_LIBSSH_VERSION}
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

find_package(Libssh2 REQUIRED)

include_directories("./")
include_directories("./include")

file(GLOB SOURCES 
    "./include/ssh/SshSocket.hpp"
    "./include/ssh/SshSession.hpp"
    "./include/ssh/SshChannel.hpp"
    "./include/KeyToNote.hpp"
    "./include/SshWrapper.hpp"
    "./include/Mikrotik.hpp"
    "./src/ssh/SshSocket.cpp"
    "./src/ssh/SshSession.cpp"
    "./src/ssh/SshChannel.cpp"
    "./src/KeyToNote.cpp"
    "./src/SshWrapper.cpp"
    "./src/Mikrotik.cpp"
    "./src/mikrotik-ssh-piano.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

IF(MSVC)
    set_property(
        TARGET 
        ${PROJECT_NAME} 
        PROPERTY 
        MSVC_RUNTIME_LIBRARY 
        "MultiThreaded$<$<CONFIG:>:>"
    )
ENDIF(MSVC)

include_directories(${Libssh2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Libssh2::libssh2)
