cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build.")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

project(network-monitor)

# Add the local CMake modules folder to the CMake search path.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Dependencies
# If dependencies are managed with conan, we prepend the current build folder
# to the CMake module path, where CMake looks for dependencies.
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()
find_package(Boost 1.74 REQUIRED COMPONENTS system)
find_package(cpr REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

# Called before any other target is defined.
enable_testing()

# On Windows, we define a preprocessor symbol with the OS version to prevent
# warnings from the Boost.Asio header files.
if(WIN32)
    include(GetWinVer REQUIRED)
    get_win_ver(WINDOWS_VERSION)
endif()

set(LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WebsocketClient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/file-downloader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/transport-network.cpp"
)
add_library(network-monitor STATIC ${LIB_SOURCES})

target_compile_features(network-monitor
    PUBLIC
        cxx_std_17
)

target_compile_definitions(network-monitor
    PRIVATE
        $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=${WINDOWS_VERSION}>
)
target_include_directories(network-monitor
    PUBLIC
        inc
)
target_link_libraries(network-monitor
    PUBLIC
        Boost::Boost
        OpenSSL::OpenSSL
        jsoncpp::jsoncpp
    PRIVATE
        cpr::cpr
)



# Tests
set(TESTS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/file-downloader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/WebsocketClient.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/transport-network.cpp"
    
)
add_executable(network-monitor-tests ${TESTS_SOURCES})
target_compile_features(network-monitor-tests
    PRIVATE
        cxx_std_17
)
target_compile_definitions(network-monitor-tests
    PRIVATE
        TESTS_CACERT_PEM="${CMAKE_CURRENT_SOURCE_DIR}/tests/cacert.pem"
        TESTS_NETWORK_LAYOUT_JSON="${CMAKE_CURRENT_SOURCE_DIR}/tests/network-layout.json"
        TEST_DATA="${CMAKE_CURRENT_SOURCE_DIR}/tests/test-data"
        $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=${WINDOWS_VERSION}>
)
target_link_libraries(network-monitor-tests
    PRIVATE
        network-monitor
        Boost::Boost
        OpenSSL::OpenSSL
        jsoncpp::jsoncpp
        
)

add_test(
    NAME network-monitor-tests
    COMMAND $<TARGET_FILE:network-monitor-tests>
)
# This tells CMake to check for a specific output to verify the test outcome.
# When all unit tests pass, Boost.Test prints "No errors detected".
set_tests_properties(network-monitor-tests PROPERTIES
    PASS_REGULAR_EXPRESSION ".*No errors detected"
)