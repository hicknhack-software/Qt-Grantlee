project(template_dirs_test)

cmake_minimum_required(VERSION 2.8.9)

find_package(Grantlee REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(
  ${Grantlee_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}
)

configure_file(grantlee_paths.h.cmake ${PROJECT_BINARY_DIR}/grantlee_paths.h)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/template_dir_1 ${PROJECT_BINARY_DIR}/template_dir_1)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/template_dir_2 ${PROJECT_BINARY_DIR}/template_dir_2)

set(template_dirs_test_SRCS
  main.cpp
)

add_executable(template_dirs_test
  ${template_dirs_test_SRCS}
)

target_link_libraries(
  template_dirs_test
  ${Grantlee_TEMPLATES_LIBRARIES}
)

qt5_use_modules(template_dirs_test Core Gui)

