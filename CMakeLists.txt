cmake_minimum_required(VERSION 3.14)
project(app VERSION 1.0.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  ecewo
  GIT_REPOSITORY https://github.com/timwmillard/ecewo.git
  GIT_TAG working
  # GIT_TAG v3.1.0
  GIT_SHALLOW TRUE
  PATCH_COMMAND patch -p1 -N < ${CMAKE_SOURCE_DIR}/patches/fix-bind-segfault.patch || true
  # COMMAND patch -p1 -N < ${CMAKE_SOURCE_DIR}/patches/fix-macos-reuseport.patch || true
)

file(DOWNLOAD
  https://raw.githubusercontent.com/tsoding/flag.h/refs/heads/master/flag.h
  ${CMAKE_SOURCE_DIR}/vendor/flag.h
)

FetchContent_Declare(
  sqlite3
  URL https://sqlite.org/2025/sqlite-amalgamation-3500400.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(ecewo sqlite3)

add_executable(${PROJECT_NAME}
  src/main.c
  ${sqlite3_SOURCE_DIR}/sqlite3.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/vendor
  ${sqlite3_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ecewo)
