
find_package(Qt5 5.0.0 REQUIRED Core Gui)

add_subdirectory(lib)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
