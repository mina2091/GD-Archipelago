cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GD-Archipelago VERSION 1.0.0)

# Add all source files inside src (recursively)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

# Optional: explizites Linken einer vorgebauten Cocos2d‑Bibliothek (z.B. universal lib mit arm64/x86_64)
# Setze in deiner CI oder lokal die Umgebungsvariablen:
#   COCOS2D_LIB -> Pfad zur libCocos2d.a (universal/arm64 enthalten)
#   COCOS2D_INC -> Pfad zum include-Verzeichnis von Cocos2d
if(DEFINED ENV{COCOS2D_LIB})
    message(STATUS "Linking with external Cocos2d library: $ENV{COCOS2D_LIB}")
    add_library(Cocos2d STATIC IMPORTED)
    set_target_properties(Cocos2d PROPERTIES
        IMPORTED_LOCATION "$ENV{COCOS2D_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "$ENV{COCOS2D_INC}"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE Cocos2d)
endif()
