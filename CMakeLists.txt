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

# Download SQLite amalgamation to vendor folder
set(SQLITE_VERSION 3500400)
set(SQLITE_YEAR 2025)
set(SQLITE_URL "https://sqlite.org/${SQLITE_YEAR}/sqlite-amalgamation-${SQLITE_VERSION}.zip")
set(SQLITE_ARCHIVE "${CMAKE_BINARY_DIR}/sqlite-amalgamation-${SQLITE_VERSION}.zip")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/vendor/sqlite3.c")
  file(DOWNLOAD ${SQLITE_URL} ${SQLITE_ARCHIVE})
  file(ARCHIVE_EXTRACT INPUT ${SQLITE_ARCHIVE} DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY
    ${CMAKE_BINARY_DIR}/sqlite-amalgamation-${SQLITE_VERSION}/sqlite3.c
    ${CMAKE_BINARY_DIR}/sqlite-amalgamation-${SQLITE_VERSION}/sqlite3.h
    DESTINATION ${CMAKE_SOURCE_DIR}/vendor
  )
endif()

FetchContent_MakeAvailable(ecewo)

add_executable(${PROJECT_NAME}
  src/main.c
  ${CMAKE_SOURCE_DIR}/vendor/sqlite3.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(${PROJECT_NAME} PRIVATE ecewo)
